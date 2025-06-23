/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/DirectX/BufferImpl.h>
#include <LibWebGPUNative/DirectX/CommandBufferImpl.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>
#include <LibWebGPUNative/DirectX/QueueImpl.h>
#include <directx/d3dx12.h>

namespace WebGPUNative {

Queue::Impl::Impl(Device const& gpu_device)
    : m_device(gpu_device.m_impl->device())
    , m_command_queue(gpu_device.m_impl->command_queue())

{
}

ErrorOr<void> Queue::Impl::submit(Vector<NonnullRawPtr<CommandBuffer>> const& gpu_command_buffers)
{
    Vector<ID3D12CommandList*> command_lists;
    for (auto const& command_buffer : gpu_command_buffers) {
        command_lists.append(command_buffer->m_impl->command_buffer().Get());
    }
    m_command_queue->ExecuteCommandLists(command_lists.size(), command_lists.data());

    ComPtr<ID3D12Fence> fence;
    if (HRESULT const result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)); FAILED(result))
        return make_error(result, "Unable to create fence for command buffer submission");

    // FIXME: Queue submission should be asynchronous
    //  https://www.w3.org/TR/webgpu/#dom-gpuqueue-onsubmittedworkdone
    HANDLE fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fence_event)
        return make_error("Unable to create fence event for command buffer submission");
    if (HRESULT const result = m_command_queue->Signal(fence.Get(), 1); FAILED(result))
        return make_error(result, "Unable to signal fence");

    if (fence->GetCompletedValue() < 1) {
        fence->SetEventOnCompletion(1, fence_event);
        WaitForSingleObject(fence_event, INFINITE);
    }

    // FIXME: Consult spec for the recommended method of notifying the GPUCanvasContext that is needs to update it's HTMLCanvasElement surface
    if (m_submitted_callback)
        m_submitted_callback();

    return {};
}

void Queue::Impl::on_submitted(Function<void()> callback)
{
    m_submitted_callback = std::move(callback);
}

ErrorOr<void> Queue::Impl::write_buffer(NonnullRawPtr<Buffer> const& buffer, [[maybe_unused]] size_t const buffer_offset, Vector<float> const& data, [[maybe_unused]] size_t const data_offset, size_t const size)
{
    auto const& buffer_impl = *buffer->m_impl;
    ComPtr<ID3D12Resource> dest_buffer = buffer_impl.buffer();

    const CD3DX12_HEAP_PROPERTIES upload_heap_props(D3D12_HEAP_TYPE_UPLOAD);
    const CD3DX12_RESOURCE_DESC dest_buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(size);
    ComPtr<ID3D12Resource> staging_buffer;
    if (HRESULT const result = m_device->CreateCommittedResource(&upload_heap_props, D3D12_HEAP_FLAG_NONE, &dest_buffer_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&staging_buffer)); FAILED(result)) {
        Device::Impl::log_debug_info(m_device);
        return make_error(result, "Unable to create device buffer");
    }

    void* mapped_data = nullptr;
    const CD3DX12_RANGE read_range(0, 0);

    if (HRESULT const result = staging_buffer->Map(0, &read_range, &mapped_data); FAILED(result)) {
        Device::Impl::log_debug_info(m_device);
        return make_error(result, "Unable to map staging upload buffer");
    }

    memcpy(mapped_data, data.data(), size);
    staging_buffer->Unmap(0, nullptr);
    return {};
}

}
