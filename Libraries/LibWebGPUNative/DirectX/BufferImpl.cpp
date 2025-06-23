/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/DirectX/BufferImpl.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>
#include <directx/d3dx12.h>

namespace WebGPUNative {

Buffer::Impl::Impl(Device const& gpu_device, BufferDescriptor const& gpu_buffer_descriptor)
    : m_device(gpu_device.m_impl->device())
    , m_buffer_descriptor(gpu_buffer_descriptor)
{
}

ErrorOr<void> Buffer::Impl::initialize()
{
    // FIXME: Don't hardcode these settings
    const CD3DX12_HEAP_PROPERTIES default_heap_props(D3D12_HEAP_TYPE_DEFAULT);
    const CD3DX12_RESOURCE_DESC vertex_buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(m_buffer_descriptor.size * sizeof(float));
    if (HRESULT const result = m_device->CreateCommittedResource(&default_heap_props, D3D12_HEAP_FLAG_NONE, &vertex_buffer_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_buffer)); FAILED(result)) {
        Device::Impl::log_debug_info(m_device);
        return make_error(result, "Unable to create device buffer");
    }

    return {};
}

}
