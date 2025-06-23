/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "RenderPipelineImpl.h"

#include <LibWebGPUNative/DirectX/BufferImpl.h>
#include <LibWebGPUNative/DirectX/CommandEncoderImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>
#include <LibWebGPUNative/DirectX/RenderPassEncoderImpl.h>

namespace WebGPUNative {

RenderPassEncoder::Impl::Impl(CommandEncoder const& gpu_command_encoder, RenderPassDescriptor const& gpu_render_pass_descriptor)
    : m_command_list(gpu_command_encoder.m_impl->command_list())
    , m_render_pass_descriptor(gpu_render_pass_descriptor)
{
}

// NOTE: There is no explicit render pass object to create for DirectX, everything is through the command list
ErrorOr<void> RenderPassEncoder::Impl::initialize()
{
    return {};
}

void RenderPassEncoder::Impl::set_pipeline(NonnullRawPtr<RenderPipeline> const& gpu_render_pipeline)
{
    auto const& render_pipeline = *gpu_render_pipeline->m_impl;
    m_command_list->SetPipelineState(render_pipeline.pipeline_state().Get());
    m_command_list->SetGraphicsRootSignature(render_pipeline.root_signature().Get());
    m_command_list->IASetPrimitiveTopology(render_pipeline.primitive_topology());
}

void RenderPassEncoder::Impl::set_vertex_buffer([[maybe_unused]] size_t slot, NonnullRawPtr<Buffer> const& buffer)
{
    auto const& buffer_impl = *buffer->m_impl;
    auto const& vertex_buffer = buffer_impl.buffer();
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
    vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
    // FIXME: Query stride from the buffer descriptor
    static constexpr size_t vertex_stride = 32;
    vertex_buffer_view.StrideInBytes = vertex_stride;
    vertex_buffer_view.SizeInBytes = buffer_impl.descriptor().size * sizeof(float);
    m_command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
}

void RenderPassEncoder::Impl::draw(size_t const vertex_count)
{
    m_command_list->DrawInstanced(vertex_count, 1, 0, 0);
}

void RenderPassEncoder::Impl::end()
{
}

}
