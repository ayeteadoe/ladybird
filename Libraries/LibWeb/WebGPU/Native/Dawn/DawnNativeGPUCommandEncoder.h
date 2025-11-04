/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUCommandEncoder.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUCommandEncoder::Impl {
    wgpu::CommandEncoder m_command_encoder { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);

    NativeGPURenderPassEncoder begin_render_pass(GPURenderPassDescriptor const& descriptor);

    NativeGPUCommandBuffer finish(Optional<GPUCommandBufferDescriptor> descriptor);
};

}
