/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeDrawingBuffer.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeDrawingBuffer::Impl {
    RefPtr<Gfx::PaintingSurface> m_surface { nullptr };
    wgpu::SharedTextureMemory m_shared_texture_memory { nullptr };
    wgpu::SharedTextureMemoryProperties m_shared_texture_memory_properties {};
    wgpu::Texture m_texture { nullptr };
};

}
