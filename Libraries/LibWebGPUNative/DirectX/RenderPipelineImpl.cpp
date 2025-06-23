/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ShaderModuleImpl.h"

#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>
#include <LibWebGPUNative/DirectX/RenderPipelineImpl.h>
#include <directx/d3dx12.h>

namespace WebGPUNative {

RenderPipeline::Impl::Impl(Device const& gpu_device, RenderPipelineDescriptor const& gpu_render_pipeline_descriptor)
    : m_device(gpu_device.m_impl->device())
    , m_render_pipeline_descriptor(gpu_render_pipeline_descriptor)

{
}

ErrorOr<void> RenderPipeline::Impl::initialize()
{
    // FIXME: Don't hardcode these settings
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC::Init_1_2(root_signature_desc, 0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature_blob;
    ComPtr<ID3DBlob> error_blob;

    if (HRESULT const result = D3DX12SerializeVersionedRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1_2, signature_blob.GetAddressOf(), error_blob.GetAddressOf()); FAILED(result)) {
        auto const error_string = ByteString(static_cast<char const*>(error_blob->GetBufferPointer()), error_blob->GetBufferSize());
        dbgln(error_string);
        return make_error(result, "Unable to serialize versioned root signature");
    }

    if (HRESULT const result = m_device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)); FAILED(result))
        return make_error(result, "Unable to root signature");

    D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};

    pipeline_state_desc.InputLayout.pInputElementDescs = input_element_descs;
    pipeline_state_desc.InputLayout.NumElements = _countof(input_element_descs);

    pipeline_state_desc.pRootSignature = m_root_signature.Get();

    auto const& vertex_state = m_render_pipeline_descriptor.vertex;
    auto const vertex_shader = vertex_state.module->m_impl->vertex(vertex_state.entry_point);
    if (vertex_shader == nullptr)
        return Error::from_string_literal("Unable to set vertex shader for render pipeline");
    pipeline_state_desc.VS = CD3DX12_SHADER_BYTECODE(vertex_shader->blob.Get());

    auto const& fragment_state = m_render_pipeline_descriptor.fragment;
    if (fragment_state.has_value()) {
        auto const fragment_shader = fragment_state.value().module->m_impl->fragment(fragment_state.value().entry_point);
        if (fragment_shader == nullptr)
            return Error::from_string_literal("Unable to set fragment shader for render pipeline");
        pipeline_state_desc.PS = CD3DX12_SHADER_BYTECODE(fragment_shader->blob.Get());
    }

    pipeline_state_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    pipeline_state_desc.RasterizerState.FrontCounterClockwise = FALSE;

    pipeline_state_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    pipeline_state_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipeline_state_desc.DepthStencilState.DepthEnable = TRUE;
    pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    pipeline_state_desc.SampleMask = UINT_MAX;

    pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    pipeline_state_desc.NumRenderTargets = 1;
    pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipeline_state_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    pipeline_state_desc.SampleDesc.Count = 1;
    pipeline_state_desc.SampleDesc.Quality = 0;

    if (HRESULT const result = m_device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&m_pipeline_state)); FAILED(result)) {
        Device::Impl::log_debug_info(m_device);
        return make_error(result, "Unable to create render pipeline");
    }
    return {};
}

}
