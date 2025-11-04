/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "DawnNativeGPUCommandBuffer.h"
#include "DawnNativeGPURenderPassEncoder.h"
#include "DawnNativeGPUTextureView.h"

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUCommandEncoder.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUCommandEncoder);

// https://www.w3.org/TR/webgpu/#dom-gpucommandencoder-beginrenderpass
NativeGPURenderPassEncoder NativeGPUCommandEncoder::Impl::begin_render_pass(GPURenderPassDescriptor const& descriptor)
{
    wgpu::RenderPassDescriptor render_pass_encoder_descriptor {};
    Vector<wgpu::RenderPassColorAttachment> render_pass_color_attachments;
    for (auto const& color_attachment : descriptor.color_attachments) {
        wgpu::RenderPassColorAttachment wgpu_color_attachment {};
        wgpu_color_attachment.view = color_attachment.view->native_gpu_texture_view().m_impl->m_texture_view;
        auto const& clear_value = color_attachment.clear_value;
        if (clear_value.has_value()) {
            clear_value.value().visit(
                [&wgpu_color_attachment](Vector<double> const& cv) {
                    VERIFY(cv.size() == 4);
                    wgpu_color_attachment.clearValue.r = cv[0];
                    wgpu_color_attachment.clearValue.g = cv[1];
                    wgpu_color_attachment.clearValue.b = cv[2];
                    wgpu_color_attachment.clearValue.a = cv[3];
                },
                [&wgpu_color_attachment](GPUColorDict const& cv) {
                    wgpu_color_attachment.clearValue.r = cv.r;
                    wgpu_color_attachment.clearValue.g = cv.g;
                    wgpu_color_attachment.clearValue.b = cv.b;
                    wgpu_color_attachment.clearValue.a = cv.a;
                });
        }
        switch (color_attachment.load_op) {
        case Bindings::GPULoadOp::Clear:
            wgpu_color_attachment.loadOp = wgpu::LoadOp::Clear;
            break;
        case Bindings::GPULoadOp::Load:
            wgpu_color_attachment.loadOp = wgpu::LoadOp::Load;
            break;
        default:
            break;
        }
        switch (color_attachment.store_op) {
        case Bindings::GPUStoreOp::Store:
            wgpu_color_attachment.storeOp = wgpu::StoreOp::Store;
            break;
        case Bindings::GPUStoreOp::Discard:
            wgpu_color_attachment.storeOp = wgpu::StoreOp::Discard;
            break;
        default:
            break;
        }
        render_pass_color_attachments.append(wgpu_color_attachment);
    }
    render_pass_encoder_descriptor.colorAttachmentCount = descriptor.color_attachments.size();
    render_pass_encoder_descriptor.colorAttachments = render_pass_color_attachments.data();

    // FIXME: Implement specification

    auto render_pass_encoder = NativeGPURenderPassEncoder::create();
    render_pass_encoder.m_impl->m_render_pass_encoder = m_command_encoder.BeginRenderPass(&render_pass_encoder_descriptor);
    return render_pass_encoder;
}

// https://www.w3.org/TR/webgpu/#dom-gpucommandencoder-finish
NativeGPUCommandBuffer NativeGPUCommandEncoder::Impl::finish([[maybe_unused]] Optional<GPUCommandBufferDescriptor> descriptor)
{
    // FIXME: Implement specification
    wgpu::CommandBufferDescriptor command_buffer_descriptor {};
    auto command_buffer = NativeGPUCommandBuffer::create();
    command_buffer.m_impl->m_command_buffer = m_command_encoder.Finish(&command_buffer_descriptor);
    return command_buffer;
}

NativeGPUCommandEncoder NativeGPUCommandEncoder::create()
{
    return NativeGPUCommandEncoder(Impl {});
}

// NOTE: wgpu::TextureView does not have a GetLabel() method exposed

String const& NativeGPUCommandEncoder::label() const
{
    return m_impl->m_label;
}

void NativeGPUCommandEncoder::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_command_encoder.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

NativeGPURenderPassEncoder NativeGPUCommandEncoder::begin_render_pass(GPURenderPassDescriptor const& descriptor)
{
    return m_impl->begin_render_pass(descriptor);
}

NativeGPUCommandBuffer NativeGPUCommandEncoder::finish(Optional<GPUCommandBufferDescriptor> descriptor)
{
    return m_impl->finish(descriptor);
}

}
