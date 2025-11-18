/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeGPUBindGroupLayout.h"

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPURenderPipeline.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPURenderPipeline);

// https://www.w3.org/TR/webgpu/#dom-gpupipelinebase-getbindgrouplayout
NativeGPUBindGroupLayout NativeGPURenderPipeline::Impl::get_bind_group_layout(WebIDL::UnsignedLong index) const
{
    // FIXME: Implement specification

    auto bind_group_layout = NativeGPUBindGroupLayout::create();
    bind_group_layout.m_impl->m_bind_group_layout = m_render_pipeline.GetBindGroupLayout(index);
    return bind_group_layout;
}

NativeGPURenderPipeline NativeGPURenderPipeline::create()
{
    return NativeGPURenderPipeline(Impl {});
}

// NOTE: wgpu::RenderPipeline does not have a GetLabel() method exposed

String const& NativeGPURenderPipeline::label() const
{
    return m_impl->m_label;
}

void NativeGPURenderPipeline::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_render_pipeline.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

NativeGPUBindGroupLayout NativeGPURenderPipeline::get_bind_group_layout(WebIDL::UnsignedLong index) const
{
    return m_impl->get_bind_group_layout(index);
}

}
