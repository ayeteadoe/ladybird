/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUTexture.h>

namespace Web::WebGPU {

struct NativeGPUTexture::Impl {
    Variant</* FIXME: wgpu::Texture, */ NonnullOwnPtr<NativeDrawingBuffer>, Empty> m_texture;

    Optional<NativeDrawingBuffer const&> drawing_buffer() const;
};

}
