/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>
#include <LibWeb/WebGPU/GPURenderPassEncoder.h>
#include <LibWeb/WebGPU/GPUTextureView.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCommandEncoder);

wgpu::CommandEncoderDescriptor GPUCommandEncoderDescriptor::to_wgpu() const
{
    return wgpu::CommandEncoderDescriptor {};
}

struct GPUCommandEncoder::Impl {
    wgpu::CommandEncoder command_encoder = { nullptr };
    String label;
};

GPUCommandEncoder::GPUCommandEncoder(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUCommandEncoder::~GPUCommandEncoder() = default;

JS::ThrowCompletionOr<GC::Ref<GPUCommandEncoder>> GPUCommandEncoder::create(JS::Realm& realm, wgpu::CommandEncoder command_encoder)
{
    return realm.create<GPUCommandEncoder>(realm, Impl { .command_encoder = move(command_encoder), .label = ""_string });
}

void GPUCommandEncoder::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCommandEncoder);
    Base::initialize(realm);
}

void GPUCommandEncoder::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::CommandEncoder GPUCommandEncoder::as_wgpu() const
{
    return m_impl->command_encoder;
}

String const& GPUCommandEncoder::label() const
{
    return m_impl->label;
}

void GPUCommandEncoder::set_label(String const& label)
{
    m_impl->label = label;
}

// https://www.w3.org/TR/webgpu/#dom-gpucommandencoder-beginrenderpass
// FIXME: Spec comments
GC::Ref<GPURenderPassEncoder> GPUCommandEncoder::begin_render_pass(GPURenderPassDescriptor const& descriptor) const
{
    wgpu::RenderPassDescriptor render_pass_encoder_descriptor {};
    Vector<wgpu::RenderPassColorAttachment> render_pass_color_attachments;
    for (auto const& color_attachment : descriptor.color_attachments) {
        wgpu::RenderPassColorAttachment wgpu_color_attachment {};
        wgpu_color_attachment.view = color_attachment.view->wgpu();
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
    // FIXME: Finish impl

    wgpu::RenderPassEncoder native_render_pass_encoder = m_impl->command_encoder.BeginRenderPass(&render_pass_encoder_descriptor);
    auto& realm = this->realm();
    return MUST(GPURenderPassEncoder::create(realm, move(native_render_pass_encoder)));
}

// https://www.w3.org/TR/webgpu/#dom-gpucommandencoder-finish
// FIXME: Spec comments
GC::Ref<GPUCommandBuffer> GPUCommandEncoder::finish(GPUCommandBufferDescriptor const& descriptor)
{
    wgpu::CommandBufferDescriptor command_buffer_descriptor = descriptor.to_wgpu();
    wgpu::CommandBuffer native_command_buffer = m_impl->command_encoder.Finish(&command_buffer_descriptor);
    auto& realm = this->realm();
    return MUST(GPUCommandBuffer::create(realm, move(native_command_buffer)));
}

}
