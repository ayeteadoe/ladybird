/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "webgpu/include/webgpu/webgpu.h"

#include <LibWeb/WebGPU/Native/WGPU/GPUSurface.h>

struct wl_display;
struct wl_surface;

namespace Web::WebGPU::Native::WGPU {

class GPUInstance;

class GPUSurface {
public:
    explicit GPUSurface(GPUInstance& instance, struct wl_display* display, struct wl_surface* surface);

    WGPUSurface& get() { return m_surface; }

private:
    WGPUSurface m_surface;
    [[maybe_unused]] WGPUSurfaceConfiguration m_surface_config;
};

}
