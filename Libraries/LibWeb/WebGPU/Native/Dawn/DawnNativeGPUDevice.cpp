/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUDevice.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUDevice);

NativeGPUDevice NativeGPUDevice::create()
{
    return NativeGPUDevice(Impl {});
}

// NOTE: wgpu::Device does not have a GetLabel() method exposed

String const& NativeGPUDevice::label() const
{
    return m_impl->m_label;
}

void NativeGPUDevice::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_device.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

}
