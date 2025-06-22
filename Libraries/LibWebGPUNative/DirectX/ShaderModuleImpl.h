/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Vector.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/ShaderModule.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace WebGPUNative {

struct VertexShader {
    String entry_point;
    ComPtr<ID3DBlob> blob;
};
struct FragmentShader {
    String entry_point;
    ComPtr<ID3DBlob> blob;
};
using ShaderTarget = Variant<VertexShader, FragmentShader>;

struct ShaderModule::Impl {
    explicit Impl(Device const& gpu_device, ShaderModuleDescriptor const&);

    ErrorOr<void> initialize();

    Vector<ShaderTarget> const& shader_targets() const { return m_shader_targets; }

private:
    ComPtr<ID3D12Device> m_device;
    Vector<ShaderTarget> m_shader_targets;
    ShaderModuleDescriptor m_shader_module_descriptor;
};

}
