/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <AK/Assertions.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUDevice.h>

namespace Web::WebGPU::Native::WGPU {

GPUDevice::GPUDevice(WGPUDevice device)
    : m_device(device)
    , m_queue(wgpuDeviceGetQueue(m_device))
{
    VERIFY(m_device);
}

}
