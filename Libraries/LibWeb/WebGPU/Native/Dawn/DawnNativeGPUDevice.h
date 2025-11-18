/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUDevice.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUDevice::Impl {
    wgpu::Device m_device { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);

    NativeGPUQueue queue() const;

    NativeGPUBuffer create_buffer(GPUBufferDescriptor const& descriptor) const;

    NativeGPUBindGroup create_bind_group(GPUBindGroupDescriptor const& descriptor) const;

    NativeGPUShaderModule create_shader_module(GPUShaderModuleDescriptor const& descriptor) const;

    NativeGPURenderPipeline create_render_pipeline(GPURenderPipelineDescriptor const& descriptor) const;

    NativeGPUCommandEncoder create_command_encoder(Optional<GPUCommandEncoderDescriptor> descriptor) const;
};

}
