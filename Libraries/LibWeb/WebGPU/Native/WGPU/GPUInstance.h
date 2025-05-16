/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "webgpu/include/webgpu/webgpu.h"
#include <AK/Function.h>
#include <LibWeb/WebGPU/Native/GPU.h>

struct wl_display;
struct wl_surface;

namespace Web::WebGPU::Native::WGPU {

class GPUSurface;
class GPUAdapter;

class GPUInstance : public Native::GPU<GPUInstance> {
public:
    explicit GPUInstance();

    GPUSurface create_surface(struct wl_display* display, struct wl_surface* surface);

    void request_adapter(GPUSurface& surface, AK::Function<void(GPUAdapter)> callback);

    WGPUInstance& get() { return m_instance; }

private:
    WGPUInstance m_instance;

    static void handle_request_adapter(WGPURequestAdapterStatus status,
        WGPUAdapter adapter, WGPUStringView message,
        void* userdata1, void* userdata2);
    AK::Function<void(GPUAdapter)> m_adapter_callback { nullptr };
};

}
