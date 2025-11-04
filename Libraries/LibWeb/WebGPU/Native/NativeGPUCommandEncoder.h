/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUCommandEncoderPrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/NativeGPURenderPassEncoder.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

struct GPUCommandEncoderDescriptor : GPUObjectDescriptorBase {
};

class NativeGPUDevice;

class NativeGPUCommandEncoder {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUCommandEncoder);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUCommandEncoder);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUCommandEncoder);

public:
    friend NativeGPUDevice;

    static NativeGPUCommandEncoder create();

    NativeGPURenderPassEncoder begin_render_pass(GPURenderPassDescriptor const& descriptor);
};

}
