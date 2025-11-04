/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeGPUCommandBuffer.h"

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUQueue.h>

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

}
