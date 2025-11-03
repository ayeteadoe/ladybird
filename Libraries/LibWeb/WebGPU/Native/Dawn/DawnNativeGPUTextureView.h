/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUTextureView.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUTextureView::Impl {
    wgpu::TextureView m_texture_view { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);
};

}
