/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/HTML/HTMLCanvasElement.h>
#include <LibWeb/HTML/OffscreenCanvas.h>
#include <LibWeb/HTML/TraversableNavigable.h>
#include <LibWeb/Painting/Paintable.h>
#include <LibWeb/WebGPU/GPUCanvasContext.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebGPU/GPUTexture.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCanvasContext);

GPUCanvasContext::GPUCanvasContext(JS::Realm& realm, HTML::HTMLCanvasElement& html_canvas, NonnullRefPtr<Gfx::SkiaBackendContext> const& skia_backend_context)
    : PlatformObject(realm)
    , m_canvas(html_canvas)
    , m_skia_backend_context(skia_backend_context)
    , m_size(html_canvas.width(), html_canvas.height())
{
}

JS::ThrowCompletionOr<GC::Ref<GPUCanvasContext>> GPUCanvasContext::create(JS::Realm& realm, HTML::HTMLCanvasElement& html_canvas)
{
    RefPtr<Gfx::SkiaBackendContext> skia_backend_context = html_canvas.navigable()->traversable_navigable()->skia_backend_context();

    if (!skia_backend_context)
        return realm.vm().throw_completion<JS::InternalError>("Unable to retrieve skia backend context for canvas element"sv);
    return realm.create<GPUCanvasContext>(realm, html_canvas, *skia_backend_context);
}

void GPUCanvasContext::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCanvasContext);
    Base::initialize(realm);
}

void GPUCanvasContext::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_canvas);
    if (m_configuration.has_value())
        visitor.visit(m_configuration->device);
    visitor.visit(m_current_texture);
}

void GPUCanvasContext::reset_to_default_state()
{
}

void GPUCanvasContext::set_size(Gfx::IntSize const& size)
{
    m_size = size;
    replace_drawing_buffer();
}

void GPUCanvasContext::allocate_painting_surface_if_needed()
{
    if (m_drawing_buffer || m_size.is_empty() || !m_configuration.has_value())
        return;

    m_drawing_buffer = MUST(NativeDrawingBuffer::create(m_skia_backend_context, m_configuration->device->native_gpu_device(), m_size));

    if (auto* paintable = m_canvas->paintable()) {
        paintable->set_needs_display();
    }
}

RefPtr<Gfx::PaintingSurface> GPUCanvasContext::surface()
{
    if (!m_drawing_buffer)
        return nullptr;
    return m_drawing_buffer->surface();
}

// https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-configure
void GPUCanvasContext::configure(GPUCanvasConfiguration const& configuration)
{
    // 1. Let device be configuration.device.
    [[maybe_unused]] auto device = configuration.device;

    // FIXME: 2. ? Validate texture format required features of configuration.format with device.[[device]].
    // FIXME: 3. ? Validate texture format required features of each element of configuration.viewFormats with device.[[device]].
    // FIXME: 4. If Supported context formats does not contain configuration.format, throw a TypeError.
    // FIXME: 5. Let descriptor be the GPUTextureDescriptor for the canvas and configuration(this.canvas, configuration).

    // 6. Set this.[[configuration]] to configuration.
    m_configuration = configuration;

    // FIXME: 7. Set this.[[textureDescriptor]] to descriptor.

    // 8. Replace the drawing buffer of this.
    replace_drawing_buffer();

    // 9. FIXME: Issue the subsequent steps on the Device timeline of device.
}

// https://www.w3.org/TR/webgpu/#abstract-opdef-replace-the-drawing-buffer
void GPUCanvasContext::replace_drawing_buffer()
{
    // 1. Expire the current texture of context.
    expire_current_texture();

    // 2. Let configuration be context.[[configuration]].
    [[maybe_unused]] auto& configuration = m_configuration;

    // 3. Set context.[[drawingBuffer]] to a transparent black image of the same size as context.canvas.
    //      FIXME: If configuration is null, the drawing buffer is tagged with the color space "srgb". In this case, the drawing buffer will remain blank until the context is configured.
    //      FIXME: If not, the drawing buffer has the specified configuration.format and is tagged with the specified configuration.colorSpace and configuration.toneMapping.'
    m_drawing_buffer.clear();
    allocate_painting_surface_if_needed();
}

// https://www.w3.org/TR/webgpu/#abstract-opdef-expire-the-current-texture
void GPUCanvasContext::expire_current_texture()
{
    // 1. If context.[[currentTexture]] is not null:
    if (m_current_texture != nullptr) {
        // FIXME: 1. Call context.[[currentTexture]].destroy() (without destroying context.[[drawingBuffer]]) to terminate write access to the image.
        // FIXME: 2. Set context.[[currentTexture]] to null.
    }
}

}
