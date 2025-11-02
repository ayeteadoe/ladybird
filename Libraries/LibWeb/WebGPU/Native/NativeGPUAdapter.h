/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUAdapterPrototype.h>
#include <LibWeb/WebGPU/Native/NativeGPUDevice.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

struct GPURequestAdapterOptions {
    String feature_level = "core"_string;
    Bindings::GPUPowerPreference power_preference;
    bool force_fallback_adapter = false;
};

class NativeGPU;

class NativeGPUAdapter {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUAdapter);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUAdapter);

public:
    friend NativeGPU;

    static NativeGPUAdapter create();

    // https://www.w3.org/TR/webgpu/#dom-adapter-state-slot
    enum class State {
        Valid,
        Consumed,
        Expired,
    };

    void request_device_initialization_steps(JS::Realm& realm, GC::Ref<WebIDL::Promise>& promise, Optional<GPUDeviceDescriptor> descriptor);
};

}
