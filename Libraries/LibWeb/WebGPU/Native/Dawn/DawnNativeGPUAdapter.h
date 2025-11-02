/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUAdapter.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUAdapter::Impl {
    wgpu::Adapter m_adapter { nullptr };

    State m_state { State::Valid };

    void request_device_initialization_steps(JS::Realm& realm, GC::Ref<WebIDL::Promise>& promise, Optional<GPUDeviceDescriptor> descriptor);
};

}
