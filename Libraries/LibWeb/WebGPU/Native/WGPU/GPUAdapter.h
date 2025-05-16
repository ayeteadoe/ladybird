/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "webgpu/include/webgpu/webgpu.h"
#include <AK/Function.h>
#include <LibWeb/WebGPU/Native/GPUAdapter.h>

namespace Web::WebGPU::Native::WGPU {

class GPUDevice;

class GPUAdapter : public Native::GPUAdapter<GPUAdapter> {
public:
    explicit GPUAdapter(WGPUAdapter adapater);

    void request_device(AK::Function<void(GPUDevice)> callback);

    void print_info() const;

private:
    WGPUAdapter m_adapter;

    static void handle_request_device(WGPURequestDeviceStatus status,
        WGPUDevice device, WGPUStringView message,
        void* userdata1, void* userdata2);
    AK::Function<void(GPUDevice)> m_device_callback { nullptr };
};

}
