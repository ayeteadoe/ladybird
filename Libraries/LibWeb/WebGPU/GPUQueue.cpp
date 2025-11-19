/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUQueue.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUQueue);

GPUQueue::GPUQueue(JS::Realm& realm, NativeGPUQueue native_gpu_queue)
    : PlatformObject(realm)
    , m_native_gpu_queue(move(native_gpu_queue))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUQueue>> GPUQueue::create(JS::Realm& realm, NativeGPUQueue native_gpu_queue)
{
    return realm.create<GPUQueue>(realm, move(native_gpu_queue));
}

void GPUQueue::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUQueue);
    Base::initialize(realm);
}

void GPUQueue::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUQueue::label() const
{
    return m_native_gpu_queue.label();
}

void GPUQueue::set_label(String const& label)
{
    m_native_gpu_queue.set_label(label);
}

// https://www.w3.org/TR/webgpu/#dom-gpuqueue-submit
void GPUQueue::submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers)
{
    m_native_gpu_queue.submit(command_buffers);
}

// https://www.w3.org/TR/webgpu/#dom-gpuqueue-writebuffer
void GPUQueue::write_buffer(GC::Root<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, Optional<WebIDL::UnsignedLongLong> data_offset, Optional<WebIDL::UnsignedLongLong> size)
{
    m_native_gpu_queue.write_buffer(move(buffer), buffer_offset, data, data_offset, size);
}

}
