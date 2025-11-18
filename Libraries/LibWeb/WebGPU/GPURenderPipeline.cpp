/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPURenderPipeline.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPURenderPipeline);

GPURenderPipeline::GPURenderPipeline(JS::Realm& realm, NativeGPURenderPipeline native_gpu_render_pipeline)
    : PlatformObject(realm)
    , m_native_gpu_render_pipeline(move(native_gpu_render_pipeline))
{
}

JS::ThrowCompletionOr<GC::Ref<GPURenderPipeline>> GPURenderPipeline::create(JS::Realm& realm, NativeGPURenderPipeline native_gpu_render_pipeline)
{
    return realm.create<GPURenderPipeline>(realm, move(native_gpu_render_pipeline));
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

String const& GPURenderPipeline::label() const
{
    return m_native_gpu_render_pipeline.label();
}

void GPURenderPipeline::set_label(String const& label)
{
    m_native_gpu_render_pipeline.set_label(label);
}

// https://www.w3.org/TR/webgpu/#dom-gpupipelinebase-getbindgrouplayout
GC::Ref<GPUBindGroupLayout> GPURenderPipeline::get_bind_group_layout(WebIDL::UnsignedLong index) const
{
    return MUST(GPUBindGroupLayout::create(realm(), m_native_gpu_render_pipeline.get_bind_group_layout(index)));
}

}
