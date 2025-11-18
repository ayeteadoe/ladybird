/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPURenderPipelinePrototype.h>
#include <LibWeb/Bindings/GPUTexturePrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/GPUShaderModule.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

struct GPUPipelineDescriptorBase : GPUObjectDescriptorBase {
    Bindings::GPUAutoLayoutMode layout;
};

struct GPUProgrammableStage {
    GC::Ptr<GPUShaderModule> module;
    Optional<String> entry_point;
};

struct GPUVertexAttribute {
    Bindings::GPUVertexFormat format;
    WebIDL::UnsignedLongLong offset;
    WebIDL::UnsignedLong shader_location;
};

struct GPUVertexBufferLayout {
    WebIDL::UnsignedLongLong array_stride;
    Bindings::GPUVertexStepMode step_mode = Bindings::GPUVertexStepMode::Vertex;
    Vector<GPUVertexAttribute> attributes;
};

struct GPUVertexState : GPUProgrammableStage {
    Vector<GPUVertexBufferLayout> buffers;
};

struct GPUPrimitiveState {
    Bindings::GPUPrimitiveTopology topology = Bindings::GPUPrimitiveTopology::TriangleList;
    Bindings::GPUIndexFormat strip_index_format;
    Bindings::GPUFrontFace front_face = Bindings::GPUFrontFace::Ccw;
    Bindings::GPUCullMode cull_mode = Bindings::GPUCullMode::None;
    bool unclipped_depth = false;
};

struct GPUDepthStencilState {
    Bindings::GPUTextureFormat format;
    Optional<bool> depth_write_enabled;
    Bindings::GPUCompareFunction depth_compare;
};

struct GPUColorTargetState {
    Bindings::GPUTextureFormat format;
};

struct GPUFragmentState : GPUProgrammableStage {
    Vector<GPUColorTargetState> targets;
};

struct GPURenderPipelineDescriptor : GPUPipelineDescriptorBase {
    GPUVertexState vertex;
    Optional<GPUPrimitiveState> primitive;
    Optional<GPUDepthStencilState> depth_stencil;
    Optional<GPUFragmentState> fragment;
};
class NativeGPUDevice;

class NativeGPURenderPipeline {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPURenderPipeline);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPURenderPipeline);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPURenderPipeline);

public:
    friend NativeGPUDevice;

    static NativeGPURenderPipeline create();
};

}
