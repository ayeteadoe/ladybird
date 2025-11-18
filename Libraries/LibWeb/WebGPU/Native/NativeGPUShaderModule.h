/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUShaderModulePrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

struct GPUShaderModuleDescriptor : GPUObjectDescriptorBase {
    String code;
};

class NativeGPUDevice;

class NativeGPUShaderModule {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUShaderModule);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUShaderModule);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUShaderModule);

public:
    friend NativeGPUDevice;

    static NativeGPUShaderModule create();
};

}
