/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeDrawingBuffer.h"

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUTexture.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUTextureView.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUTexture);

// https://www.w3.org/TR/webgpu/#dom-gputexture-createview
NativeGPUTextureView NativeGPUTexture::Impl::create_view([[maybe_unused]] Optional<GPUTextureViewDescriptor> descriptor)
{
    auto view = NativeGPUTextureView::create();
    // AD-HOC: Use textures default descriptor when we are a drawing buffer, as we need to ensure we match the configuration of the implicitly created shared texture memory
    if (m_is_drawing_buffer)
        view.m_impl->m_texture_view = m_texture.CreateView();

    // FIXME: Implement specification
    return view;
}

NativeGPUTexture NativeGPUTexture::create_from_drawing_buffer(NativeDrawingBuffer& drawing_buffer)
{
    return NativeGPUTexture(Impl { .m_texture = drawing_buffer.m_impl->m_texture, .m_is_drawing_buffer = true });
}

bool NativeGPUTexture::is_drawing_buffer() const
{
    return m_impl->m_is_drawing_buffer;
}

NativeGPUTextureView NativeGPUTexture::create_view(Optional<GPUTextureViewDescriptor> descriptor)
{
    return m_impl->create_view(descriptor);
}

}
