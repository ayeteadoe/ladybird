/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWebGPUNative/Buffer.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace WebGPUNative {

struct Buffer::Impl {
    explicit Impl(Device const& gpu_device, BufferDescriptor const&);

    ErrorOr<void> initialize();

    ComPtr<ID3D12Resource> buffer() const { return m_buffer; }

    BufferDescriptor const& descriptor() const { return m_buffer_descriptor; }

    // TODO: Implement map_buffer to be used in Queue::write_buffer, similar to Texture::map_buffer

    // TODO: RenderPassEncoder::set_vertex_buffer() needs to create the vertex buffer view  from the host_buffer()

private:
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_buffer;
    BufferDescriptor m_buffer_descriptor;
};

}
