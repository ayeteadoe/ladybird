/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/ArrayBuffer.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUBuffer.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUBuffer);

WebIDL::UnsignedLongLong NativeGPUBuffer::Impl::size() const
{
    return m_buffer.GetSize();
}

WebIDL::UnsignedLong NativeGPUBuffer::Impl::usage() const
{
    return static_cast<WebIDL::UnsignedLong>(m_buffer.GetUsage());
}

Bindings::GPUBufferMapState NativeGPUBuffer::Impl::map_state() const
{
    switch (m_buffer.GetMapState()) {
    case wgpu::BufferMapState::Unmapped:
        return Bindings::GPUBufferMapState::Unmapped;
    case wgpu::BufferMapState::Pending:
        return Bindings::GPUBufferMapState::Pending;
    case wgpu::BufferMapState::Mapped:
        return Bindings::GPUBufferMapState::Mapped;
    default:
        VERIFY_NOT_REACHED();
    }
}

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-getmappedrange
GC::Root<JS::ArrayBuffer> NativeGPUBuffer::Impl::get_mapped_range(JS::Realm& realm, Optional<WebIDL::UnsignedLongLong> offset, Optional<WebIDL::UnsignedLongLong> size) const
{
    auto range_offset = offset.value_or(0);

    // 1. If size is missing:
    //      1. Let rangeSize be max(0, this.size - offset).
    //    Otherwise, let rangeSize be size.
    auto range_size = size.value_or(max(0, m_buffer.GetSize() - range_offset));

    // FIXME: Implement remaining specification

    void* mapped_range = m_buffer.GetMappedRange(range_offset, range_size);
    VERIFY(mapped_range);

    auto mapped_ranged_buffer = MUST(ByteBuffer::copy(mapped_range, range_size));
    return JS::ArrayBuffer::create(realm, mapped_ranged_buffer);
}

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-unmap
void NativeGPUBuffer::Impl::unmap()
{
    // FIXME: Implement specification
    m_buffer.Unmap();
}

NativeGPUBuffer NativeGPUBuffer::create()
{
    return NativeGPUBuffer(Impl {});
}

// NOTE: wgpu::Buffer does not have a GetLabel() method exposed

String const& NativeGPUBuffer::label() const
{
    return m_impl->m_label;
}

void NativeGPUBuffer::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_buffer.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

WebIDL::UnsignedLongLong NativeGPUBuffer::size() const
{
    return m_impl->size();
}

WebIDL::UnsignedLong NativeGPUBuffer::usage() const
{
    return m_impl->usage();
}

Bindings::GPUBufferMapState NativeGPUBuffer::map_state() const
{
    return m_impl->map_state();
}

GC::Root<JS::ArrayBuffer> NativeGPUBuffer::get_mapped_range(JS::Realm& realm, Optional<WebIDL::UnsignedLongLong> offset, Optional<WebIDL::UnsignedLongLong> size) const
{
    return m_impl->get_mapped_range(realm, offset, size);
}

void NativeGPUBuffer::unmap()
{
    m_impl->unmap();
}

}
