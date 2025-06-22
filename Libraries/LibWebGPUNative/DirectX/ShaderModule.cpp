/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/DirectX/ShaderModuleImpl.h>
#include <LibWebGPUNative/ShaderModule.h>

namespace WebGPUNative {

ShaderModule::ShaderModule(Device const& gpu_device, ShaderModuleDescriptor const& gpu_shader_module_descriptor)
    : m_impl(make<Impl>(gpu_device, gpu_shader_module_descriptor))
{
}

ShaderModule::ShaderModule(ShaderModule&&) noexcept = default;
ShaderModule& ShaderModule::operator=(ShaderModule&&) noexcept = default;
ShaderModule::~ShaderModule() = default;

ErrorOr<void> ShaderModule::initialize()
{
    return m_impl->initialize();
}

}
