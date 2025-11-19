/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPURenderPassEncoderPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/GPURenderPipeline.h>
#include <LibWeb/WebGPU/Native/NativeGPURenderPassEncoder.h>

namespace Web::WebGPU {

class GPURenderPassEncoder final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPURenderPassEncoder, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPURenderPassEncoder);

    static JS::ThrowCompletionOr<GC::Ref<GPURenderPassEncoder>> create(JS::Realm&, NativeGPURenderPassEncoder);

    String const& label() const;
    void set_label(String const& label);

    NativeGPURenderPassEncoder& native_gpu_render_pass_encoder() { return m_native_gpu_render_pass_encoder; }

    void end();

    void set_pipeline(GC::Root<GPURenderPipeline> pipeline);

private:
    explicit GPURenderPassEncoder(JS::Realm&, NativeGPURenderPassEncoder);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPURenderPassEncoder m_native_gpu_render_pass_encoder;
};

}
