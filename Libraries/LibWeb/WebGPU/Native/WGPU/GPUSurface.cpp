/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <LibWeb/WebGPU/Native/WGPU/GPUInstance.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUSurface.h>

namespace Web::WebGPU::Native::WGPU {

GPUSurface::GPUSurface(GPUInstance& instance, struct wl_display* display, struct wl_surface* surface)
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
        instance.get(), &descriptor);
}

}
