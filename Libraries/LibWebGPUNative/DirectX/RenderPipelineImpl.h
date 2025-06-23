/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/RenderPipeline.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace WebGPUNative {

struct RenderPipeline::Impl {
    explicit Impl(Device const& gpu_device, RenderPipelineDescriptor const&);

    ErrorOr<void> initialize();

    ComPtr<ID3D12RootSignature> root_signature() const { return m_root_signature; }

    ComPtr<ID3D12PipelineState> pipeline_state() const { return m_pipeline_state; }

    D3D_PRIMITIVE_TOPOLOGY primitive_topology() const { return m_primitive_topology; }

private:
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12RootSignature> m_root_signature;
    ComPtr<ID3D12PipelineState> m_pipeline_state;
    D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
    RenderPipelineDescriptor m_render_pipeline_descriptor;
};

}
