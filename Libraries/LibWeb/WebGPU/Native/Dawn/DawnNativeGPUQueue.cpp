/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeGPUCommandBuffer.h"

#include <LibJS/Runtime/ArrayBuffer.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUBuffer.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUQueue.h>
#include <LibWeb/WebIDL/Buffers.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUQueue);

// https://www.w3.org/TR/webgpu/#dom-gpuqueue-submit
void NativeGPUQueue::Impl::submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers)
{
    Vector<wgpu::CommandBuffer> wgpu_command_buffers;
    for (auto const& command_buffer : command_buffers) {
        wgpu_command_buffers.append(command_buffer->native_gpu_command_buffer().m_impl->m_command_buffer);
    }

    // FIXME: Implement specification
    m_queue.Submit(command_buffers.size(), wgpu_command_buffers.data());
}

// https://www.w3.org/TR/webgpu/#dom-gpuqueue-writebuffer
void NativeGPUQueue::Impl::write_buffer(GC::Root<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, /*FIXME: Dawn does not expose this field*/ [[maybe_unused]] Optional<WebIDL::UnsignedLongLong> data_offset, Optional<WebIDL::UnsignedLongLong> size)
{
    // Content timeline steps:
    // FIXME: 1. If data is an ArrayBuffer or DataView, let the element type be "byte". Otherwise, data is a TypedArray; let the element type be the type of the TypedArray.
    ByteBuffer const data_buffer = data->viewed_array_buffer()->buffer();

    // 2. Let dataSize be the size of data, in elements.
    auto const data_size = data_buffer.size();

    // 3. If size is missing, let contentsSize be dataSize − dataOffset. Otherwise, let contentsSize be size.
    auto const contents_size = size.has_value() ? size.value() : data_size - data_offset.value_or(0);

    // FIXME: 4. If any of the following conditions are unsatisfied, throw an OperationError and return.
    // FIXME: 5. Let dataContents be a copy of the bytes held by the buffer source data.
    // FIXME: 6. Let contents be the contentsSize elements of dataContents starting at an offset of dataOffset elements.
    // FIXME: 7. Issue the subsequent steps on the Device timeline of this.

    // Device timeline steps:
    // FIXME: 1. If any of the following conditions are unsatisfied, generate a validation error and return.
    // FIXME: 2. Issue the subsequent steps on the Queue timeline of this.

    // Queue timeline steps:
    // FIXME: 1. Write contents into buffer starting at bufferOffset.
    m_queue.WriteBuffer(buffer->native_gpu_buffer().m_impl->m_buffer, buffer_offset, data_buffer.data(), contents_size);
}

NativeGPUQueue NativeGPUQueue::create()
{
    return NativeGPUQueue(Impl {});
}

// NOTE: wgpu::TextureView does not have a GetLabel() method exposed

String const& NativeGPUQueue::label() const
{
    return m_impl->m_label;
}

void NativeGPUQueue::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_queue.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

void NativeGPUQueue::submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers)
{
    m_impl->submit(command_buffers);
}

void NativeGPUQueue::write_buffer(GC::Root<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, Optional<WebIDL::UnsignedLongLong> data_offset, Optional<WebIDL::UnsignedLongLong> size)
{
    m_impl->write_buffer(move(buffer), buffer_offset, data, data_offset, size);
}

}
