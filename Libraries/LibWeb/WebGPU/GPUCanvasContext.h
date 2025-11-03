/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/PaintingSurface.h>
#include <LibWeb/Bindings/GPUCanvasContextPrototype.h>
#include <LibWeb/Bindings/GPUTexturePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeDrawingBuffer.h>

namespace Web::WebGPU {

struct GPUCanvasConfiguration {
    GC::Ptr<GPUDevice> device;
    Bindings::GPUTextureFormat format;
};

class GPUCanvasContext final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCanvasContext, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCanvasContext);

    static JS::ThrowCompletionOr<GC::Ref<GPUCanvasContext>> create(JS::Realm&, HTML::HTMLCanvasElement&);

    void reset_to_default_state();

    void set_size(Gfx::IntSize const&);

    void allocate_painting_surface_if_needed();

    RefPtr<Gfx::PaintingSurface> surface();

    GC::Ref<HTML::HTMLCanvasElement> canvas() const { return m_canvas; }

    void configure(GPUCanvasConfiguration const& configuration);

    GC::Ptr<GPUTexture> get_current_texture();

private:
    explicit GPUCanvasContext(JS::Realm&, HTML::HTMLCanvasElement&, NonnullRefPtr<Gfx::SkiaBackendContext> const&);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    void replace_drawing_buffer();

    void expire_current_texture();

    void update_the_rendering();

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-canvas
    GC::Ref<HTML::HTMLCanvasElement> m_canvas;

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-configuration-slot
    Optional<GPUCanvasConfiguration> m_configuration;

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-texturedescriptor-slot

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-drawingbuffer-slot
    NonnullRefPtr<Gfx::SkiaBackendContext> m_skia_backend_context;
    OwnPtr<NativeDrawingBuffer> m_drawing_buffer { nullptr };

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-currenttexture-slot
    GC::Ptr<GPUTexture> m_current_texture;

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-lastpresentedimage-slot

    Gfx::IntSize m_size;
};

}
