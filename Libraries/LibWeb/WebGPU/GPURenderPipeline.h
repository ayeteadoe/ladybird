/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUPrototype.h>
#include <LibWeb/Bindings/GPURenderPipelinePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class RenderPipeline;
struct RenderPipelineDescriptor;

}

namespace Web::WebGPU {

struct GPUPipelineDescriptorBase : GPUObjectDescriptorBase {
    Bindings::GPUAutoLayoutMode layout;
};

struct GPUProgrammableStage {
    GC::Ptr<GPUShaderModule> module;
    Optional<String> entry_point;
};

struct GPUVertexState : GPUProgrammableStage {
};

struct GPUPrimitiveState {
    Bindings::GPUPrimitiveTopology topology = Bindings::GPUPrimitiveTopology::TriangleList;
    Bindings::GPUIndexFormat strip_index_format;
    Bindings::GPUFrontFace front_face = Bindings::GPUFrontFace::Ccw;
    Bindings::GPUCullMode cull_mode = Bindings::GPUCullMode::None;
    bool unclipped_depth = false;
};

struct GPUColorTargetState {
    Bindings::GPUTextureFormat format;
};

struct GPUFragmentState : GPUProgrammableStage {
    Vector<GPUColorTargetState> targets;
};

struct GPURenderPipelineDescriptor : GPUPipelineDescriptorBase {
    GPUVertexState vertex;
    GPUPrimitiveState primitive {};
    // FIXME: GPUDepthStencilState depth_stencil;
    // FIXME: GPUMultisampleState multisample {};
    GPUFragmentState fragment;
};

class GPURenderPipeline final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPURenderPipeline, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPURenderPipeline);

    static JS::ThrowCompletionOr<GC::Ref<GPURenderPipeline>> create(JS::Realm&, wgpu::RenderPipeline);

    ~GPURenderPipeline() override;

    wgpu::RenderPipeline wgpu() const;

    String const& label() const;
    void set_label(String const& label);

private:
    struct Impl;
    explicit GPURenderPipeline(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
