/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUPrototype.h>
#include <LibWeb/Bindings/GPURenderPassEncoderPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class RenderPassEncoder;
struct RenderPassDescriptor;

}

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

struct GPURenderPassDescriptor : GPUObjectDescriptorBase {
    Vector<GPURenderPassColorAttachment> color_attachments;
};

class GPURenderPassEncoder final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPURenderPassEncoder, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPURenderPassEncoder);

    static JS::ThrowCompletionOr<GC::Ref<GPURenderPassEncoder>> create(JS::Realm&, wgpu::RenderPassEncoder);

    ~GPURenderPassEncoder() override;

    wgpu::RenderPassEncoder as_wgpu() const;

    String const& label() const;
    void set_label(String const& label);

    void set_pipeline(GC::Ref<GPURenderPipeline> render_pipeline);

    void draw(GPUSize32 vertex_count, GPUSize32 instance_count = 1, GPUSize32 first_vertex = 0, GPUSize32 first_instance = 0);

    void end();

private:
    struct Impl;
    explicit GPURenderPassEncoder(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
