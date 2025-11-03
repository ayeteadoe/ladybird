/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUTexture.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUTexture::Impl {
    wgpu::Texture m_texture;
    bool m_is_drawing_buffer { false };

    NativeGPUTextureView create_view(Optional<GPUTextureViewDescriptor> descriptor);
};

}
