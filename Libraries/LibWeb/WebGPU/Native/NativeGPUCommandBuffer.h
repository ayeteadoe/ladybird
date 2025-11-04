/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUCommandBufferPrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

struct GPUCommandBufferDescriptor : GPUObjectDescriptorBase {
};

class NativeGPUCommandEncoder;

class NativeGPUCommandBuffer {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUCommandBuffer);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUCommandBuffer);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUCommandBuffer);

public:
    friend NativeGPUCommandEncoder;

    static NativeGPUCommandBuffer create();
};

}
