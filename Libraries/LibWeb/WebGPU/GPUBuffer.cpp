/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUBuffer.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUBuffer);

GPUBuffer::GPUBuffer(JS::Realm& realm, NativeGPUBuffer native_gpu_buffer)
    : PlatformObject(realm)
    , m_native_gpu_buffer(move(native_gpu_buffer))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUBuffer>> GPUBuffer::create(JS::Realm& realm, NativeGPUBuffer native_gpu_texture_view)
{
    return realm.create<GPUBuffer>(realm, move(native_gpu_texture_view));
}

void GPUBuffer::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUBuffer);
    Base::initialize(realm);
}

void GPUBuffer::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUBuffer::label() const
{
    return m_native_gpu_buffer.label();
}

void GPUBuffer::set_label(String const& label)
{
    m_native_gpu_buffer.set_label(label);
}

WebIDL::UnsignedLongLong GPUBuffer::size() const
{
    return m_native_gpu_buffer.size();
}

WebIDL::UnsignedLong GPUBuffer::usage() const
{
    return m_native_gpu_buffer.usage();
}

Bindings::GPUBufferMapState GPUBuffer::map_state() const
{
    return m_native_gpu_buffer.map_state();
}

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-getmappedrange
GC::Root<JS::ArrayBuffer> GPUBuffer::get_mapped_range(Optional<WebIDL::UnsignedLongLong> offset, Optional<WebIDL::UnsignedLongLong> size) const
{
    return m_native_gpu_buffer.get_mapped_range(realm(), offset, size);
}

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-unmap
void GPUBuffer::unmap()
{
    m_native_gpu_buffer.unmap();
}

}
