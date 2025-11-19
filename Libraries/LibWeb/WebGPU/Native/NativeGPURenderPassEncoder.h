/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPURenderPassEncoderPrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/GPUTextureView.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

struct GPUColorDict {
    double r;
    double g;
    double b;
    double a;
};
using GPUColor = Variant<Vector<double>, GPUColorDict>;

struct GPURenderPassColorAttachment {
    GC::Ptr<GPUTextureView> view;

    Optional<GPUColor> clear_value;
    Bindings::GPULoadOp load_op;
    Bindings::GPUStoreOp store_op;
};

struct GPURenderPassDepthStencilAttachment {
    GC::Ptr<GPUTextureView> view;

    Optional<float> depth_clear_value;
    Optional<Bindings::GPULoadOp> depth_load_op;
    Optional<Bindings::GPUStoreOp> depth_store_op;
};

struct GPURenderPassDescriptor : GPUObjectDescriptorBase {
    Vector<GPURenderPassColorAttachment> color_attachments;
    Optional<GPURenderPassDepthStencilAttachment> depth_stencil_attachment;
};

class NativeGPUCommandEncoder;

class NativeGPURenderPassEncoder {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPURenderPassEncoder);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPURenderPassEncoder);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPURenderPassEncoder);

public:
    friend NativeGPUCommandEncoder;

    static NativeGPURenderPassEncoder create();

    void end();

    void set_pipeline(GC::Root<GPURenderPipeline> pipeline);

    void draw(WebIDL::UnsignedLong vertex_count, WebIDL::UnsignedLong instance_count, WebIDL::UnsignedLong first_vertex, WebIDL::UnsignedLong first_instance);
};

}
