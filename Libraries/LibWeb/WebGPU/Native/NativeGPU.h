/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUAdapter.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

class NativeGPU {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPU);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPU);

public:
    static NativeGPU create();

    void request_adapter_initialization_steps(JS::Realm& realm, GC::Ref<WebIDL::Promise>& promise, Optional<GPURequestAdapterOptions> options);
};

}
