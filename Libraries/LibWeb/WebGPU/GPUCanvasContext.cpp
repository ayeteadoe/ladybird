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
#include <LibWeb/WebGPU/GPUCanvasContext.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCanvasContext);

GPUCanvasContext::GPUCanvasContext(JS::Realm& realm, HTML::HTMLCanvasElement& html_canvas, NonnullRefPtr<Gfx::SkiaBackendContext> const& skia_backend_context)
    : PlatformObject(realm)
    , m_canvas(html_canvas)
    , m_skia_backend_context(skia_backend_context)
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
}

void GPUCanvasContext::reset_to_default_state()
{
}

void GPUCanvasContext::set_size(Gfx::IntSize const&)
{
    // FIXME: https://www.w3.org/TR/webgpu/#abstract-opdef-replace-the-drawing-buffer
}

void GPUCanvasContext::allocate_painting_surface_if_needed()
{
    // FIXME: Similar to the WebGL implementation, we will use shared texture memory for displaying
    //        the contents of WebGPU render passes on the canvas painting surface
}

}
