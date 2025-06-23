/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "RenderPipelineImpl.h"

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

void RenderPassEncoder::Impl::end()
{
}

}
