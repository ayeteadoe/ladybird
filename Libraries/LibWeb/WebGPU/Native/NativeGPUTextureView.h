/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUTextureViewPrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

struct GPUTextureViewDescriptor : GPUObjectDescriptorBase {
};

class NativeGPUCommandEncoder;
class NativeGPUTexture;

class NativeGPUTextureView {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUTextureView);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUTextureView);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUTextureView);

public:
    friend NativeGPUCommandEncoder;
    friend NativeGPUTexture;

    static NativeGPUTextureView create();
};

}
