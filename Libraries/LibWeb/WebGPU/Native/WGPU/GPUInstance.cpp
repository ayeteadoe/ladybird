/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <LibWeb/WebGPU/Native/WGPU/GPUAdapter.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUInstance.h>

namespace Web::WebGPU::Native::WGPU {

GPUInstance::GPUInstance()
    : m_adapter_request_promise(MUST(Core::Promise<GPUAdapter>::try_create()))
{
}

void GPUInstance::set_surface(struct wl_display* display, struct wl_surface* surface)
{
    WGPUSurfaceSourceWaylandSurface source = {
        .chain = WGPUChainedStruct { .next = nullptr, .sType = WGPUSType_SurfaceSourceWaylandSurface },
        .display = display,
        .surface = surface
    };
    WGPUSurfaceDescriptor descriptor = {
        .nextInChain = reinterpret_cast<WGPUChainedStruct const*>(&source),
        .label = WGPUStringView { .data = nullptr, .length = WGPU_STRLEN }
    };

    m_surface = wgpuInstanceCreateSurface(
        m_instance, &descriptor);
}

AK::NonnullRefPtr<Core::Promise<GPUAdapter>> GPUInstance::request_adapter()
{
    VERIFY(m_surface != nullptr);
    WGPURequestAdapterOptions options = {};
    options.compatibleSurface = m_surface;

    WGPURequestAdapterCallbackInfo callback_info = {};
    callback_info.callback = handle_request_adapter;
    callback_info.userdata1 = this;

    wgpuInstanceRequestAdapter(m_instance,
        &options,
        callback_info);
    return m_adapter_request_promise;
}

void GPUInstance::handle_request_adapter(WGPURequestAdapterStatus status,
    [[maybe_unused]] WGPUAdapter adapter, [[maybe_unused]] WGPUStringView message,
    [[maybe_unused]] void* userdata1, [[maybe_unused]] void* userdata2)
{
    if (status == WGPURequestAdapterStatus_Success) {

        // static_cast<GPUInstance*>(userdata1)->m_adapter_request_promise->resolve(GPUAdapter(adapter));
    } else {
        // FIXME: Log error?
    }
}

}
