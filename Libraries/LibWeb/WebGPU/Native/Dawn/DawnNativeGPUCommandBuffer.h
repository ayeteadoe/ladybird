/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUCommandBuffer.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUCommandBuffer::Impl {
    wgpu::CommandBuffer m_command_buffer { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);
};

}
