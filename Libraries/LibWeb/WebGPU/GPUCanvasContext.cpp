/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGfx/ImmutableBitmap.h>
#include <LibGfx/PainterSkia.h>
#include <LibGfx/SkiaBackendContext.h>
#include <LibWeb/DOM/Document.h>
#include <LibWeb/HTML/TraversableNavigable.h>
#include <LibWeb/Painting/Paintable.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebGPU/GPUCanvasContext.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebGPU/GPUTexture.h>

#include <webgpu/webgpu_cpp.h>

// Enable WebGPU if we're on MacOS
#if defined(AK_OS_MACOS)
#    define ENABLE_WEBGPU 1
#endif

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCanvasContext);

struct GPUCanvasContext::Impl {
    GC::Ref<HTML::HTMLCanvasElement> canvas;
    Gfx::IntSize size;
    GC::Ptr<GPUDevice> device { nullptr };

    // FIXME: Use double buffering

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-drawingbuffer-slot
    NonnullRefPtr<Gfx::SkiaBackendContext> skia_backend_context;
    RefPtr<Gfx::PaintingSurface> drawing_buffer { nullptr };

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-currenttexture-slot
    GC::Ptr<GPUTexture> current_texture { nullptr };
    wgpu::SharedTextureMemory shared_texture_memory { nullptr };
    wgpu::SharedTextureMemoryProperties shared_texture_memory_properties {};
    bool shared_texture_memory_access_began { false };
};

GPUCanvasContext::GPUCanvasContext(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUCanvasContext::~GPUCanvasContext() = default;

JS::ThrowCompletionOr<GC::Ref<GPUCanvasContext>> GPUCanvasContext::create(JS::Realm& realm, HTML::HTMLCanvasElement& element, JS::Value)
{
    auto skia_backend_context = element.navigable()->traversable_navigable()->skia_backend_context();
    VERIFY(skia_backend_context);
    return realm.create<GPUCanvasContext>(realm, Impl { .canvas = element, .size = Gfx::IntSize {
                                                                               element.width(),
                                                                               element.height(),
                                                                           },
                                                     .skia_backend_context = *skia_backend_context });
}

GC::Ref<HTML::HTMLCanvasElement> GPUCanvasContext::canvas_for_binding() const
{
    return m_impl->canvas;
}

// FIXME: Add spec comments
//  https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-configure
void GPUCanvasContext::configure(GPUCanvasConfiguration const& config)
{
#if defined(ENABLE_WEBGPU)
    VERIFY(!config.device.is_null());
    m_impl->device = config.device;
    allocate_painting_surface_if_needed();
#endif
}

// FIXME: Add spec comments
//  https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-getcurrenttexture
GC::Root<GPUTexture> GPUCanvasContext::get_current_texture() const
{
#if defined(ENABLE_WEBGPU)
    if (!m_impl->shared_texture_memory_access_began) {
        wgpu::SharedTextureMemoryBeginAccessDescriptor shared_texture_memory_begin_access_descriptor {};
        shared_texture_memory_begin_access_descriptor.initialized = false;
        auto const shared_texture_memory_begin_access_result = m_impl->shared_texture_memory.BeginAccess(m_impl->current_texture->wgpu(), &shared_texture_memory_begin_access_descriptor);
        VERIFY(shared_texture_memory_begin_access_result == wgpu::Status::Success);
        m_impl->shared_texture_memory_access_began = true;
    }
    m_impl->drawing_buffer->notify_content_will_change();
    m_impl->canvas->navigable()->set_needs_repaint();
#endif
    return m_impl->current_texture;
}

// https://www.w3.org/TR/webgpu/#abstract-opdef-expire-the-current-texture
void GPUCanvasContext::expire_current_texture()
{
#if defined(ENABLE_WEBGPU)
    // To Expire the current texture of a GPUCanvasContext context, run the following content timeline steps:
    // 1. If context.[[currentTexture]] is not null:
    if (m_impl->current_texture != nullptr) {
        // 1. Call context.[[currentTexture]].destroy() (without destroying context.[[drawingBuffer]]) to terminate write access to the image.
        if (m_impl->shared_texture_memory_access_began) {
            wgpu::SharedTextureMemoryEndAccessState shared_texture_memory_end_access_state {};
            auto const shared_texture_memory_end_access_result = m_impl->shared_texture_memory.EndAccess(m_impl->current_texture->wgpu(), &shared_texture_memory_end_access_state);
            VERIFY(shared_texture_memory_end_access_result == wgpu::Status::Success);
            m_impl->shared_texture_memory_access_began = false;
        }
        m_impl->current_texture->wgpu().Destroy();
        // 2. Set context.[[currentTexture]] to null.
        m_impl->current_texture = nullptr;
    }
#endif
}

RefPtr<Gfx::PaintingSurface> GPUCanvasContext::surface()
{
    return m_impl->drawing_buffer;
}

void GPUCanvasContext::allocate_painting_surface_if_needed()
{
#if defined(ENABLE_WEBGPU)
    if (m_impl->drawing_buffer || m_impl->size.is_empty())
        return;

    // NOTE: Due to our current Navigable infrastructure only supporting a single cached SkiaBackendContext,
    // instead of creating a dawn-based SkiaBackendContext and creating a PaintingSurface for the wgpu::Texture,
    // we are initially taking the same approach as our WebGL implementation via shared texture memory for displaying
    // the contents of WebGPU render passes.
    wgpu::SharedTextureMemoryDescriptor shared_texture_memory_descriptor {};

#if defined(AK_OS_MACOS)
    auto iosurface = Core::IOSurfaceHandle::create(m_impl->size.width(), m_impl->size.height());
    m_impl->drawing_buffer = Gfx::PaintingSurface::create_from_iosurface(move(iosurface), m_impl->skia_backend_context, Gfx::PaintingSurface::Origin::TopLeft);

    wgpu::SharedTextureMemoryIOSurfaceDescriptor shared_texture_memory_io_surface_descriptor {};
    shared_texture_memory_io_surface_descriptor.ioSurface = iosurface.core_foundation_pointer();
    shared_texture_memory_descriptor.nextInChain = &shared_texture_memory_io_surface_descriptor;
#elif defined(USE_VULKAN_IMAGES)
    // FIXME: Use one of the Shared TextureMemory options available for Vulkan-based SkiaBackendContext on Linux
#elif defined(AK_OS_WINDOWS)
    // FIXME: Populate wgpu::SharedTextureMemoryDXGISharedHandleDescriptor for DirectX-based SkiaBackendContext on Windows
#endif

    m_impl->shared_texture_memory = m_impl->device->wgpu().ImportSharedTextureMemory(&shared_texture_memory_descriptor);
    auto const shared_texture_memory_properties_result = m_impl->shared_texture_memory.GetProperties(&m_impl->shared_texture_memory_properties);
    VERIFY(shared_texture_memory_properties_result == wgpu::Status::Success);

    wgpu::TextureDescriptor current_texture_descriptor {};
    current_texture_descriptor.usage = m_impl->shared_texture_memory_properties.usage;
    current_texture_descriptor.format = m_impl->shared_texture_memory_properties.format;
    current_texture_descriptor.size = m_impl->shared_texture_memory_properties.size;
    current_texture_descriptor.dimension = wgpu::TextureDimension::e2D;
    wgpu::Texture current_texture = m_impl->shared_texture_memory.CreateTexture(&current_texture_descriptor);
    VERIFY(current_texture);
    m_impl->current_texture = MUST(GPUTexture::create(realm(), move(current_texture)));

    if (auto* paintable = m_impl->canvas->paintable()) {
        paintable->set_needs_display();
    }
#endif
}

void GPUCanvasContext::set_size(Gfx::IntSize const& size)
{
    if (m_impl->size == size)
        return;
    m_impl->size = size;
    expire_current_texture();
    m_impl->drawing_buffer = nullptr;
}

void GPUCanvasContext::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_impl->canvas);
    visitor.visit(m_impl->current_texture);
}

void GPUCanvasContext::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCanvasContext);
    Base::initialize(realm);
}

}
