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

namespace Web::WebGPU {

struct GPUCanvasConfiguration {
    GC::Ref<GPUDevice> device;
    Bindings::GPUTextureFormat format;
};

class GPUCanvasContext final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCanvasContext, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCanvasContext);

    static JS::ThrowCompletionOr<GC::Ref<GPUCanvasContext>> create(JS::Realm&, HTML::HTMLCanvasElement&);

    void reset_to_default_state();

    void set_size(Gfx::IntSize const&);

    void allocate_painting_surface_if_needed();

    RefPtr<Gfx::PaintingSurface> surface() { return m_drawing_buffer; }

    GC::Ref<HTML::HTMLCanvasElement> canvas() const { return m_canvas; }

private:
    explicit GPUCanvasContext(JS::Realm&, HTML::HTMLCanvasElement&, NonnullRefPtr<Gfx::SkiaBackendContext> const&);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-canvas
    GC::Ref<HTML::HTMLCanvasElement> m_canvas;

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-configuration-slot

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-texturedescriptor-slot

    // https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-drawingbuffer-slot
    NonnullRefPtr<Gfx::SkiaBackendContext> m_skia_backend_context;
    RefPtr<Gfx::PaintingSurface> m_drawing_buffer { nullptr };

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-currenttexture-slot

    // FIXME: https://www.w3.org/TR/webgpu/#dom-gpucanvascontext-lastpresentedimage-slot

    Gfx::IntSize m_size;
};

}
