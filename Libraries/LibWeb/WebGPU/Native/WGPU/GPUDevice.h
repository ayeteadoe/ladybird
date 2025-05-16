/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "webgpu/include/webgpu/webgpu.h"

#include <LibWeb/WebGPU/Native/GPUDevice.h>

namespace Web::WebGPU::Native::WGPU {

class GPUDevice : public Native::GPUDevice<GPUDevice> {
public:
    explicit GPUDevice(WGPUDevice device);

    // TODO: Expose shader module, pipeline layout, and render pipeline functions

private:
    WGPUDevice m_device;
    WGPUQueue m_queue;
};

}
