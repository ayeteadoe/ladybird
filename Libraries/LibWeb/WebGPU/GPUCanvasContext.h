/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/Bitmap.h>
#include <LibWeb/Bindings/GPUCanvasContextPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/HTML/HTMLCanvasElement.h>

namespace Web::WebGPU {

struct GPUCanvasConfiguration {
    GC::Root<GPUDevice> device;
};

class GPUCanvasContext final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCanvasContext, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCanvasContext);

    static JS::ThrowCompletionOr<GC::Ref<GPUCanvasContext>> create(JS::Realm&, HTML::HTMLCanvasElement&, JS::Value options);

    ~GPUCanvasContext() override;

    GC::Ref<HTML::HTMLCanvasElement> canvas_for_binding() const;

    void configure(GPUCanvasConfiguration const&);

    GC::Root<GPUTexture> get_current_texture() const;

    RefPtr<Gfx::PaintingSurface> surface();
    void allocate_painting_surface_if_needed();

    void set_size(Gfx::IntSize const&);

    void reset_to_default_state() { }

private:
    struct Impl;
    explicit GPUCanvasContext(JS::Realm&, Impl);

    void expire_current_texture();

    void visit_edges(Visitor&) override;

    void initialize(JS::Realm&) override;

    NonnullOwnPtr<Impl> m_impl;
};

}
