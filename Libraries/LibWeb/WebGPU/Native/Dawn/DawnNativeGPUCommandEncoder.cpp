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
    auto to_load_op = [](Bindings::GPULoadOp load_op_binding) {
        switch (load_op_binding) {
        case Bindings::GPULoadOp::Clear:
            return wgpu::LoadOp::Clear;
        case Bindings::GPULoadOp::Load:
            return wgpu::LoadOp::Load;
        default:
            VERIFY_NOT_REACHED();
        }
    };
    auto to_store_op = [](Bindings::GPUStoreOp store_op_binding) {
        switch (store_op_binding) {
        case Bindings::GPUStoreOp::Store:
            return wgpu::StoreOp::Store;
        case Bindings::GPUStoreOp::Discard:
            return wgpu::StoreOp::Discard;
        default:
            VERIFY_NOT_REACHED();
        }
    };

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
        wgpu_color_attachment.loadOp = to_load_op(color_attachment.load_op);
        wgpu_color_attachment.storeOp = to_store_op(color_attachment.store_op);
        render_pass_color_attachments.append(wgpu_color_attachment);
    }
    render_pass_encoder_descriptor.colorAttachmentCount = descriptor.color_attachments.size();
    render_pass_encoder_descriptor.colorAttachments = render_pass_color_attachments.data();

    Optional<wgpu::RenderPassDepthStencilAttachment> maybe_depth_stencil_attachment {};
    if (auto const& depth_stencil_attachment_binding = descriptor.depth_stencil_attachment; depth_stencil_attachment_binding.has_value()) {
        wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment {};
        depth_stencil_attachment.view = depth_stencil_attachment_binding->view->native_gpu_texture_view().m_impl->m_texture_view;
        if (auto const& depth_clear_value_binding = depth_stencil_attachment_binding->depth_clear_value; depth_clear_value_binding.has_value())
            depth_stencil_attachment.depthClearValue = depth_clear_value_binding.value();
        if (auto const& depth_load_op_binding = depth_stencil_attachment_binding->depth_load_op; depth_load_op_binding.has_value())
            depth_stencil_attachment.depthLoadOp = to_load_op(depth_load_op_binding.value());
        if (auto const& depth_store_op_binding = depth_stencil_attachment_binding->depth_store_op; depth_store_op_binding.has_value())
            depth_stencil_attachment.depthStoreOp = to_store_op(depth_store_op_binding.value());

        maybe_depth_stencil_attachment = depth_stencil_attachment;
    }
    if (maybe_depth_stencil_attachment.has_value())
        render_pass_encoder_descriptor.depthStencilAttachment = &maybe_depth_stencil_attachment.value();

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
