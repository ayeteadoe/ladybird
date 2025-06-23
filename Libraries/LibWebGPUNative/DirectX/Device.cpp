/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Buffer.h>
#include <LibWebGPUNative/CommandEncoder.h>
#include <LibWebGPUNative/Device.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/Queue.h>
#include <LibWebGPUNative/RenderPipeline.h>
#include <LibWebGPUNative/ShaderModule.h>
#include <LibWebGPUNative/Texture.h>

namespace WebGPUNative {

Device::Device(Adapter const& gpu_adapter)
    : m_impl(make<Impl>(gpu_adapter))
{
}

Device::Device(Device&&) noexcept = default;
Device& Device::operator=(Device&&) noexcept = default;
Device::~Device() = default;

ErrorOr<void> Device::initialize()
{
    return m_impl->initialize();
}

Queue Device::queue() const
{
    return Queue(*this);
}

Buffer Device::buffer(BufferDescriptor const& gpu_buffer_descriptor) const
{
    return Buffer(*this, gpu_buffer_descriptor);
}

Texture Device::texture(Gfx::IntSize const size) const
{
    return Texture(*this, size);
}

CommandEncoder Device::command_encoder() const
{
    return CommandEncoder(*this);
}

RenderPipeline Device::render_pipeline(RenderPipelineDescriptor const& gpu_render_pipeline_descriptor) const
{
    return RenderPipeline(*this, gpu_render_pipeline_descriptor);
}

ShaderModule Device::shader_module(ShaderModuleDescriptor const& gpu_shader_module_descriptor) const
{
    return ShaderModule(*this, gpu_shader_module_descriptor);
}

}
