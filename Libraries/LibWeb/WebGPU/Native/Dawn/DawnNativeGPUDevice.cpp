/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUCommandEncoder.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUDevice.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUDevice);

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createcommandencoder
NativeGPUCommandEncoder NativeGPUDevice::Impl::create_command_encoder([[maybe_unused]] Optional<GPUCommandEncoderDescriptor> descriptor) const
{
    // FIXME: Implement specification

    auto command_encoder = NativeGPUCommandEncoder::create();
    wgpu::CommandEncoderDescriptor command_encoder_descriptor {};
    command_encoder.m_impl->m_command_encoder = m_device.CreateCommandEncoder(&command_encoder_descriptor);
    return command_encoder;
}

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

NativeGPUCommandEncoder NativeGPUDevice::create_command_encoder(Optional<GPUCommandEncoderDescriptor> descriptor) const
{
    return m_impl->create_command_encoder(descriptor);
}

}
