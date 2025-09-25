/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPURenderPipeline.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPURenderPipeline);

struct GPURenderPipeline::Impl {
    wgpu::RenderPipeline render_pipeline = { nullptr };
    String label;
};

GPURenderPipeline::GPURenderPipeline(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPURenderPipeline::~GPURenderPipeline() = default;

JS::ThrowCompletionOr<GC::Ref<GPURenderPipeline>> GPURenderPipeline::create(JS::Realm& realm, wgpu::RenderPipeline render_pipeline)
{
    return realm.create<GPURenderPipeline>(realm, Impl { .render_pipeline = move(render_pipeline) });
}

void GPURenderPipeline::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPURenderPipeline);
    Base::initialize(realm);
}

void GPURenderPipeline::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::RenderPipeline GPURenderPipeline::wgpu() const
{
    return m_impl->render_pipeline;
}

String const& GPURenderPipeline::label() const
{
    return m_impl->label;
}

void GPURenderPipeline::set_label(String const& label)
{
    m_impl->label = label;
}

}
