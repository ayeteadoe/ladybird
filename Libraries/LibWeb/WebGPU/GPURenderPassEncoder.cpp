/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPURenderPassEncoder.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPURenderPassEncoder);

GPURenderPassEncoder::GPURenderPassEncoder(JS::Realm& realm, NativeGPURenderPassEncoder native_gpu_render_pass_encoder)
    : PlatformObject(realm)
    , m_native_gpu_render_pass_encoder(move(native_gpu_render_pass_encoder))
{
}

JS::ThrowCompletionOr<GC::Ref<GPURenderPassEncoder>> GPURenderPassEncoder::create(JS::Realm& realm, NativeGPURenderPassEncoder native_gpu_render_pass_encoder)
{
    return realm.create<GPURenderPassEncoder>(realm, move(native_gpu_render_pass_encoder));
}

void GPURenderPassEncoder::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPURenderPassEncoder);
    Base::initialize(realm);
}

void GPURenderPassEncoder::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPURenderPassEncoder::label() const
{
    return m_native_gpu_render_pass_encoder.label();
}

void GPURenderPassEncoder::set_label(String const& label)
{
    m_native_gpu_render_pass_encoder.set_label(label);
}

// https://www.w3.org/TR/webgpu/#dom-gpurenderpassencoder-end
void GPURenderPassEncoder::end()
{
    m_native_gpu_render_pass_encoder.end();
}

// https://www.w3.org/TR/webgpu/#dom-gpurendercommandsmixin-setpipeline
void GPURenderPassEncoder::set_pipeline(GC::Root<GPURenderPipeline> pipeline)
{
    m_native_gpu_render_pass_encoder.set_pipeline(move(pipeline));
}

// https://www.w3.org/TR/webgpu/#dom-gpurendercommandsmixin-draw
void GPURenderPassEncoder::draw(WebIDL::UnsignedLong vertex_count, WebIDL::UnsignedLong instance_count, WebIDL::UnsignedLong first_vertex, WebIDL::UnsignedLong first_instance)
{
    m_native_gpu_render_pass_encoder.draw(vertex_count, instance_count, first_vertex, first_instance);
}

}
