/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUTexture.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUTexture);

Optional<NativeDrawingBuffer const&> NativeGPUTexture::Impl::drawing_buffer() const
{
    if (auto* drawing_buffer = m_texture.get_pointer<NonnullOwnPtr<NativeDrawingBuffer>>())
        return *drawing_buffer->ptr();
    return {};
}

NativeGPUTexture NativeGPUTexture::create_from_drawing_buffer(NonnullOwnPtr<NativeDrawingBuffer> drawing_buffer)
{
    return NativeGPUTexture(Impl { .m_texture = move(drawing_buffer) });
}

Optional<NativeDrawingBuffer const&> NativeGPUTexture::drawing_buffer() const
{
    return m_impl->drawing_buffer();
}

}
