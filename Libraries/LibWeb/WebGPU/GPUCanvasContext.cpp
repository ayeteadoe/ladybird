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
#include <LibWeb/WebGPU/GPUQueue.h>
#include <LibWeb/WebGPU/GPUTexture.h>

#include <gpu/graphite/dawn/DawnBackendContext.h>
#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCanvasContext);

struct GPUCanvasContext::Impl {

    GC::Ref<HTML::HTMLCanvasElement> canvas;
    Gfx::IntSize size;

    GC::Ptr<GPUDevice> device;

    // FIXME: Support triple buffering
    GC::Ptr<GPUTexture> current_texture;

    OwnPtr<skgpu::graphite::DawnBackendContext> dawn_context;
    RefPtr<Gfx::SkiaBackendContext> context;
    RefPtr<Gfx::PaintingSurface> surface;
    OwnPtr<Gfx::Painter> painter;
};

GPUCanvasContext::GPUCanvasContext(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUCanvasContext::~GPUCanvasContext() = default;

JS::ThrowCompletionOr<GC::Ref<GPUCanvasContext>> GPUCanvasContext::create(JS::Realm& realm, HTML::HTMLCanvasElement& element, JS::Value)
{
    return realm.create<GPUCanvasContext>(realm, Impl {
                                                     .canvas = element,
                                                     .size = Gfx::IntSize { element.width(), element.height() },
                                                 });
}

GC::Ref<HTML::HTMLCanvasElement> GPUCanvasContext::canvas_for_binding() const
{
    return m_impl->canvas;
}

// FIXME: Add spec comments
//  https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-configure
void GPUCanvasContext::configure(GPUCanvasConfiguration const& config)
{
    // allocate_painting_surface_if_needed();
    VERIFY(!config.device.is_null());

    m_impl->device = config.device;
    auto queue = m_impl->device->queue();

    m_impl->dawn_context = adopt_own_if_nonnull(new skgpu::graphite::DawnBackendContext {
        .fInstance = m_impl->device->instance()->wgpu(),
        .fDevice = m_impl->device->wgpu(),
        .fQueue = queue->wgpu(),
        .fTick = skgpu::graphite::DawnNativeProcessEventsFunction });

    m_impl->context = Gfx::SkiaBackendContext::create_dawn_context(*m_impl->dawn_context);

    // auto submitted_promise = config.device->queue()->on_submitted_work_done();

    /*
    config.device->on_queue_submitted([this]() {
        // FIXME: Follow spec guidelines for how to update the canvas drawing buffer
        //  https://www.w3.org/TR/webgpu/#abstract-opdef-get-a-copy-of-the-image-contents-of-a-context

        m_surface->notify_content_will_change();
        auto const mapped_texture_buffer = MUST(m_current_texture->map_buffer());
        for (auto const& [pixel, x, y] : mapped_texture_buffer->pixels()) {
            m_bitmap->set_pixel(x, y, pixel);
        }
        update_display();
    });

    m_current_texture = config.device->texture(m_size);
    */

    /*
    m_surface->notify_content_will_change();
    constexpr auto transparent_black = Color(0, 0, 0, 0);
    for (int y = 0; y < m_bitmap->height(); ++y) {
        for (int x = 0; x < m_bitmap->width(); ++x) {
            m_bitmap->set_pixel(x, y, transparent_black);
        }
    }
    update_display();
    */

    allocate_painting_surface_if_needed();
}

// FIXME: Add spec comments
//  https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-getcurrenttexture
GC::Root<GPUTexture> GPUCanvasContext::get_current_texture() const
{
    return m_impl->current_texture;
}

void GPUCanvasContext::present()
{
    dbgln("GPUCanvasContext::present");

    if (auto* paintable = m_impl->canvas->paintable()) {
        paintable->set_needs_display();
    }
}

RefPtr<Gfx::PaintingSurface> GPUCanvasContext::surface()
{
    return m_impl->surface;
}

void GPUCanvasContext::allocate_painting_surface_if_needed()
{
    if (m_impl->surface || m_impl->size.is_empty())
        return;

    GPUTextureDescriptor current_texture_descriptor {};
    current_texture_descriptor.usage = to_underlying(wgpu::TextureUsage::RenderAttachment) | to_underlying(wgpu::TextureUsage::CopySrc);
    current_texture_descriptor.dimension = Bindings::GPUTextureDimension::_2d;
    current_texture_descriptor.format = Bindings::GPUTextureFormat::Bgra8unorm;
    current_texture_descriptor.size = GPUExtent3D { .width = static_cast<uint32_t>(m_impl->size.width()), .height = static_cast<uint32_t>(m_impl->size.height()) };
    m_impl->current_texture = m_impl->device->create_texture(current_texture_descriptor);

    VERIFY(!m_impl->context.is_null());
    m_impl->surface = Gfx::PaintingSurface::create_from_wgputexture(*m_impl->context, m_impl->current_texture->as_wgpu());

    // TODO: Temp infra to test rendering content from GPU commands into the texture actually displays said content on the canvas
    dbgln("Starting GPU render pass test...");
    wgpu::CommandEncoder command_encoder = m_impl->device->wgpu().CreateCommandEncoder();

    wgpu::TextureViewDescriptor texture_view_descriptor {};
    texture_view_descriptor.format = m_impl->current_texture->as_wgpu().GetFormat();
    texture_view_descriptor.dimension = wgpu::TextureViewDimension::e2D;
    texture_view_descriptor.baseMipLevel = 0;
    texture_view_descriptor.mipLevelCount = 1;
    texture_view_descriptor.aspect = wgpu::TextureAspect::All;
    texture_view_descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    wgpu::TextureView texture_view = m_impl->current_texture->as_wgpu().CreateView();

    wgpu::RenderPassColorAttachment color_attachment {};
    color_attachment.view = texture_view;
    color_attachment.loadOp = wgpu::LoadOp::Clear;
    color_attachment.storeOp = wgpu::StoreOp::Store;
    color_attachment.clearValue = wgpu::Color{.r = 1.0, .g = 0.0, .b = 0.0, .a = 1.0};

    wgpu::RenderPassDescriptor render_pass_descriptor {};
    render_pass_descriptor.colorAttachmentCount = 1;
    render_pass_descriptor.colorAttachments = &color_attachment;
    wgpu::RenderPassEncoder render_pass_encoder = command_encoder.BeginRenderPass(&render_pass_descriptor);
    render_pass_encoder.End();

    wgpu::CommandBuffer command_buffer = command_encoder.Finish();
    m_impl->device->queue()->wgpu().Submit(1, &command_buffer);

    // Testing if this actually does properly display the content, or if we still need to do something do actually update the SKCanvas content/display like DisplayListPlayerSkia for our graphite-based PaintingSurface
    //m_impl->context->flush_and_submit(&m_impl->surface->sk_surface());

    dbgln("Finished GPU render pass test");
    // TODO: End test
}

void GPUCanvasContext::set_size(Gfx::IntSize const& size)
{
    if (m_impl->size == size)
        return;
    m_impl->size = size;
    m_impl->surface = nullptr;
}

void GPUCanvasContext::update_display() const
{
    /*
        if (auto* paintable = m_impl->canvas->paintable()) {
            paintable->set_needs_display();
        }
    */
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
