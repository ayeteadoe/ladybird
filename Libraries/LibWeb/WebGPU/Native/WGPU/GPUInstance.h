/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "webgpu/include/webgpu/webgpu.h"
#include <LibCore/Promise.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUAdapter.h>

struct wl_display;
struct wl_surface;

namespace Web::WebGPU::Native::WGPU {

class GPUInstance {
public:
    GPUInstance();

    void set_surface(struct wl_display* display, struct wl_surface* surface);

    WGPUInstance& get() { return m_instance; }

    AK::NonnullRefPtr<Core::Promise<GPUAdapter>> request_adapter();

private:
    static void handle_request_adapter(WGPURequestAdapterStatus status,
        WGPUAdapter adapter, WGPUStringView message,
        void* userdata1, void* userdata2);

    WGPUInstance m_instance { wgpuCreateInstance(nullptr) };

    WGPUSurface m_surface;
    [[maybe_unused]] WGPUSurfaceConfiguration m_surface_config;

    AK::NonnullRefPtr<Core::Promise<GPUAdapter>> m_adapter_request_promise;
};

}
