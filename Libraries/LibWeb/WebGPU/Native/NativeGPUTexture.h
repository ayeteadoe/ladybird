/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUTexturePrototype.h>
#include <LibWeb/WebGPU/Native/NativeDrawingBuffer.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

class NativeGPUTexture {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUTexture);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUTexture);

public:
    // We distinguish between creating a texture for a shared drawing buffer vs a texture from a standard texture descriptor to ensure the texture view descriptor that will be passed to rendering pass is compatible with the drawing buffer texture
    static NativeGPUTexture create_from_drawing_buffer(NativeDrawingBuffer& drawing_buffer);

    bool is_drawing_buffer() const;
};

}
