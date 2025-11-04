/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPURenderPassEncoder.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPURenderPassEncoder);

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

}
