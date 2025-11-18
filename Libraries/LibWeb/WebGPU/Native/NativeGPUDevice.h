/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUDevicePrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/NativeGPUBuffer.h>
#include <LibWeb/WebGPU/Native/NativeGPUCommandEncoder.h>
#include <LibWeb/WebGPU/Native/NativeGPURenderPipeline.h>
#include <LibWeb/WebGPU/Native/NativeGPUShaderModule.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

struct GPUDeviceDescriptor : GPUObjectDescriptorBase {
};

class NativeDrawingBuffer;
class NativeGPUAdapter;

class NativeGPUDevice {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUDevice);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUDevice);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUDevice);

public:
    friend NativeDrawingBuffer;
    friend NativeGPUAdapter;

    static NativeGPUDevice create();

    NativeGPUQueue queue() const;

    NativeGPUBuffer create_buffer(GPUBufferDescriptor const& descriptor) const;

    NativeGPUShaderModule create_shader_module(GPUShaderModuleDescriptor const& descriptor) const;

    NativeGPURenderPipeline create_render_pipeline(GPURenderPipelineDescriptor const& descriptor) const;

    NativeGPUCommandEncoder create_command_encoder(Optional<GPUCommandEncoderDescriptor> descriptor) const;
};

}
