/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUBindGroup.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUBindGroup::Impl {
    wgpu::BindGroup m_bind_group { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);
};

}
