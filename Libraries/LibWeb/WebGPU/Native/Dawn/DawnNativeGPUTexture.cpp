/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeDrawingBuffer.h"

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUTexture.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUTexture);

NativeGPUTexture NativeGPUTexture::create_from_drawing_buffer(NativeDrawingBuffer& drawing_buffer)
{
    return NativeGPUTexture(Impl { .m_texture = drawing_buffer.m_impl->m_texture, .m_is_drawing_buffer = true });
}

bool NativeGPUTexture::is_drawing_buffer() const
{
    return m_impl->m_is_drawing_buffer;
}

}
