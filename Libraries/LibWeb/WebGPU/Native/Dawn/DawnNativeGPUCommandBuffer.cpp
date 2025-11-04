/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUCommandBuffer.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUCommandBuffer);

NativeGPUCommandBuffer NativeGPUCommandBuffer::create()
{
    return NativeGPUCommandBuffer(Impl {});
}

// NOTE: wgpu::CommandBuffer does not have a GetLabel() method exposed

String const& NativeGPUCommandBuffer::label() const
{
    return m_impl->m_label;
}

void NativeGPUCommandBuffer::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_command_buffer.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

}
