/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUBindGroupLayout.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUBindGroupLayout::Impl {
    wgpu::BindGroupLayout m_bind_group_layout { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);
};

}
