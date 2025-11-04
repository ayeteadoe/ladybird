/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCommandBuffer);

GPUCommandBuffer::GPUCommandBuffer(JS::Realm& realm, NativeGPUCommandBuffer native_gpu_command_buffer)
    : PlatformObject(realm)
    , m_native_gpu_command_buffer(move(native_gpu_command_buffer))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUCommandBuffer>> GPUCommandBuffer::create(JS::Realm& realm, NativeGPUCommandBuffer native_gpu_command_buffer)
{
    return realm.create<GPUCommandBuffer>(realm, move(native_gpu_command_buffer));
}

void GPUCommandBuffer::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCommandBuffer);
    Base::initialize(realm);
}

void GPUCommandBuffer::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUCommandBuffer::label() const
{
    return m_native_gpu_command_buffer.label();
}

void GPUCommandBuffer::set_label(String const& label)
{
    m_native_gpu_command_buffer.set_label(label);
}

}
