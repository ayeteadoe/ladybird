/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCommandEncoder);

GPUCommandEncoder::GPUCommandEncoder(JS::Realm& realm, NativeGPUCommandEncoder native_gpu_command_encoder)
    : PlatformObject(realm)
    , m_native_gpu_command_encoder(move(native_gpu_command_encoder))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUCommandEncoder>> GPUCommandEncoder::create(JS::Realm& realm, NativeGPUCommandEncoder native_gpu_command_encoder)
{
    return realm.create<GPUCommandEncoder>(realm, move(native_gpu_command_encoder));
}

void GPUCommandEncoder::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCommandEncoder);
    Base::initialize(realm);
}

void GPUCommandEncoder::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUCommandEncoder::label() const
{
    return m_native_gpu_command_encoder.label();
}

void GPUCommandEncoder::set_label(String const& label)
{
    m_native_gpu_command_encoder.set_label(label);
}

// https://www.w3.org/TR/webgpu/#dom-gpucommandencoder-beginrenderpass
GC::Ref<GPURenderPassEncoder> GPUCommandEncoder::begin_render_pass(GPURenderPassDescriptor const& descriptor)
{
    return MUST(GPURenderPassEncoder::create(realm(), m_native_gpu_command_encoder.begin_render_pass(descriptor)));
}

// https://www.w3.org/TR/webgpu/#dom-gpucommandencoder-finish
GC::Ref<GPUCommandBuffer> GPUCommandEncoder::finish(Optional<GPUCommandBufferDescriptor> descriptor)
{
    return MUST(GPUCommandBuffer::create(realm(), m_native_gpu_command_encoder.finish(descriptor)));
}

}
