/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

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

class GPUPipelineBaseMixin {
public:
    virtual ~GPUPipelineBaseMixin();

    String const& label() const { return m_label; }

private:
    String m_label;
};

struct GPURenderPipelineDescriptor : GPUPipelineDescriptorBase {
};

class GPURenderPipeline final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPURenderPipeline, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPURenderPipeline);

    static JS::ThrowCompletionOr<GC::Ref<GPURenderPipeline>> create(JS::Realm&, wgpu::RenderPipeline);

    ~GPURenderPipeline() override;

    wgpu::RenderPipeline as_wgpu() const;

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
