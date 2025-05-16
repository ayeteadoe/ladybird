/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <LibWeb/WebGPU/Native/WGPU/GPUAdapter.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUDevice.h>

namespace Web::WebGPU::Native::WGPU {

GPUAdapter::GPUAdapter(WGPUAdapter adapater)
    : m_adapter(adapater)
{
}

void GPUAdapter::request_device(AK::Function<void(GPUDevice)> callback)
{
    m_device_callback = std::move(callback);
    WGPURequestDeviceCallbackInfo callback_info = {};
    callback_info.callback = handle_request_device;
    callback_info.userdata1 = this;

    wgpuAdapterRequestDevice(m_adapter, nullptr /* FIXME: Support descriptor */,
        callback_info);
}

void GPUAdapter::handle_request_device(WGPURequestDeviceStatus status,
    WGPUDevice device, [[maybe_unused]] WGPUStringView message,
    void* userdata1, [[maybe_unused]] void* userdata2)
{
    if (status == WGPURequestDeviceStatus_Success) {
        static_cast<GPUAdapter*>(userdata1)->m_device_callback(GPUDevice(device));
    } else {
        // FIXME: Log error?
    }
}

void GPUAdapter::print_info() const
{
    struct WGPUAdapterInfo info = {};
    wgpuAdapterGetInfo(m_adapter, &info);

    auto free_info = AK::ScopeGuard([&info]() {
        wgpuAdapterInfoFreeMembers(info);
    });
    printf("description: %.*s\n", (int)info.description.length, info.description.data);
    printf("vendor: %.*s\n", (int)info.vendor.length, info.vendor.data);
    printf("architecture: %.*s\n", (int)info.architecture.length, info.architecture.data);
    printf("device: %.*s\n", (int)info.device.length, info.device.data);
    printf("backend type: %u\n", info.backendType);
    printf("adapter type: %u\n", info.adapterType);
    printf("vendorID: %x\n", info.vendorID);
    printf("deviceID: %x\n", info.deviceID);
}

}
