/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPURenderPipelinePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPURenderPipeline.h>

namespace Web::WebGPU {

class GPURenderPipeline final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPURenderPipeline, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPURenderPipeline);

    static JS::ThrowCompletionOr<GC::Ref<GPURenderPipeline>> create(JS::Realm&, NativeGPURenderPipeline);

    String const& label() const;
    void set_label(String const& label);

    NativeGPURenderPipeline& native_gpu_render_pipeline() { return m_native_gpu_render_pipeline; }

private:
    explicit GPURenderPipeline(JS::Realm&, NativeGPURenderPipeline);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPURenderPipeline m_native_gpu_render_pipeline;
};

}
