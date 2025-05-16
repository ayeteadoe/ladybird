/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <LibWeb/WebGPU/Native/WGPU/GPUAdapter.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUInstance.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUSurface.h>

namespace Web::WebGPU::Native::WGPU {

GPUInstance::GPUInstance()
    : m_instance(wgpuCreateInstance(nullptr))
{
}

GPUSurface GPUInstance::create_surface(struct wl_display* display, struct wl_surface* surface)
{
    return GPUSurface(*this, display, surface);
}

void GPUInstance::request_adapter(GPUSurface& surface, AK::Function<void(GPUAdapter)> callback)
{
    m_adapter_callback = std::move(callback);
    WGPURequestAdapterOptions options = {};
    options.compatibleSurface = surface.get();

    WGPURequestAdapterCallbackInfo callback_info = {};
    callback_info.callback = handle_request_adapter;
    callback_info.userdata1 = this;

    wgpuInstanceRequestAdapter(m_instance,
        &options,
        callback_info);
}

void GPUInstance::handle_request_adapter(WGPURequestAdapterStatus status,
    WGPUAdapter adapter, [[maybe_unused]] WGPUStringView message,
    void* userdata1, [[maybe_unused]] void* userdata2)
{
    if (status == WGPURequestAdapterStatus_Success) {
        static_cast<GPUInstance*>(userdata1)->m_adapter_callback(GPUAdapter(adapter));
    } else {
        // FIXME: Log error?
    }
}

}
