/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPURenderPassEncoder.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPURenderPassEncoder::Impl {
    wgpu::RenderPassEncoder m_render_pass_encoder { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);

    void end();

    void set_pipeline(GC::Root<GPURenderPipeline> pipeline);

    void draw(WebIDL::UnsignedLong vertex_count, WebIDL::UnsignedLong instance_count, WebIDL::UnsignedLong first_vertex, WebIDL::UnsignedLong first_instance);
};

}
