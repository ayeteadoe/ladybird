/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUBindGroupLayoutPrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

class NativeGPUDevice;
class NativeGPURenderPipeline;

class NativeGPUBindGroupLayout {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUBindGroupLayout);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUBindGroupLayout);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUBindGroupLayout);

public:
    friend NativeGPUDevice;
    friend NativeGPURenderPipeline;

    static NativeGPUBindGroupLayout create();
};

}
