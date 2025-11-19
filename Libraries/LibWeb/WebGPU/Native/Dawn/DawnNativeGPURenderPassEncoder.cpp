/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeGPURenderPipeline.h"

#include <LibWeb/WebGPU/GPURenderPipeline.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPURenderPassEncoder.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPURenderPassEncoder);

// // https://www.w3.org/TR/webgpu/#dom-gpurenderpassencoder-end
void NativeGPURenderPassEncoder::Impl::end()
{
    // FIXME: Implement specification
    m_render_pass_encoder.End();
}

// https://www.w3.org/TR/webgpu/#dom-gpurendercommandsmixin-setpipeline
void NativeGPURenderPassEncoder::Impl::set_pipeline(GC::Root<GPURenderPipeline> pipeline)
{
    // FIXME: Implement specification
    m_render_pass_encoder.SetPipeline(pipeline->native_gpu_render_pipeline().m_impl->m_render_pipeline);
}

// https://www.w3.org/TR/webgpu/#dom-gpurendercommandsmixin-draw
void NativeGPURenderPassEncoder::Impl::draw(WebIDL::UnsignedLong vertex_count, WebIDL::UnsignedLong instance_count, WebIDL::UnsignedLong first_vertex, WebIDL::UnsignedLong first_instance)
{
    // FIXME: Implement specification
    m_render_pass_encoder.Draw(vertex_count, instance_count, first_vertex, first_instance);
}

NativeGPURenderPassEncoder NativeGPURenderPassEncoder::create()
{
    return NativeGPURenderPassEncoder(Impl {});
}

// NOTE: wgpu::RenderPassEncoder does not have a GetLabel() method exposed

String const& NativeGPURenderPassEncoder::label() const
{
    return m_impl->m_label;
}

void NativeGPURenderPassEncoder::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_render_pass_encoder.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

void NativeGPURenderPassEncoder::end()
{
    m_impl->end();
}

void NativeGPURenderPassEncoder::set_pipeline(GC::Root<GPURenderPipeline> pipeline)
{
    m_impl->set_pipeline(move(pipeline));
}

void NativeGPURenderPassEncoder::draw(WebIDL::UnsignedLong vertex_count, WebIDL::UnsignedLong instance_count, WebIDL::UnsignedLong first_vertex, WebIDL::UnsignedLong first_instance)
{
    m_impl->draw(vertex_count, instance_count, first_vertex, first_instance);
}

}
