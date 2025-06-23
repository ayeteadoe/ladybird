/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/DirectX/AdapterImpl.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>

namespace WebGPUNative {

Device::Impl::Impl(Adapter const& gpu_adapter)
    : m_device(gpu_adapter.m_impl->device())
{
}

ErrorOr<void> Device::Impl::initialize()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    if (HRESULT const result = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_command_queue)); FAILED(result))
        return make_error(result, "Unable to create command queue");

    if (HRESULT const result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator)); FAILED(result))
        return make_error(result, "Unable to create command allocator");
    return {};
}

void Device::Impl::log_debug_info(ComPtr<ID3D12Device> const& device)
{
    ComPtr<ID3D12InfoQueue> info_queue;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&info_queue)))) {
        const UINT64 message_count = info_queue->GetNumStoredMessages();

        for (UINT64 i = 0; i < message_count; ++i) {
            SIZE_T message_length = 0;
            info_queue->GetMessage(i, nullptr, &message_length);

            auto message = std::make_unique<BYTE[]>(message_length);
            auto d3d_message = reinterpret_cast<D3D12_MESSAGE*>(message.get());
            if (SUCCEEDED(info_queue->GetMessage(i, d3d_message, &message_length))) {
                dbgln("{}", d3d_message->pDescription);
            }
        }

        info_queue->ClearStoredMessages();
    }
}

}
