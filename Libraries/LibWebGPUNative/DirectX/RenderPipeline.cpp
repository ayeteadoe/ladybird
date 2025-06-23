/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/DirectX/RenderPipelineImpl.h>
#include <LibWebGPUNative/RenderPipeline.h>

namespace WebGPUNative {

RenderPipeline::RenderPipeline(Device const& gpu_device, RenderPipelineDescriptor const& gpu_render_pipeline_descriptor)
    : m_impl(make<Impl>(gpu_device, gpu_render_pipeline_descriptor))
{
}

RenderPipeline::RenderPipeline(RenderPipeline&&) noexcept = default;
RenderPipeline& RenderPipeline::operator=(RenderPipeline&&) noexcept = default;
RenderPipeline::~RenderPipeline() = default;

ErrorOr<void> RenderPipeline::initialize()
{
    return m_impl->initialize();
}

}
