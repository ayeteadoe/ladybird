/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUBufferPrototype.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

struct GPUBufferDescriptor : GPUObjectDescriptorBase {
    WebIDL::UnsignedLongLong size;
    WebIDL::UnsignedLong usage;
    bool mapped_at_creation = false;
};

class NativeGPUDevice;

class NativeGPUBuffer {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUBuffer);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUBuffer);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUBuffer);

public:
    friend NativeGPUDevice;

    static NativeGPUBuffer create();

    WebIDL::UnsignedLongLong size() const;
    WebIDL::UnsignedLong usage() const;
    Bindings::GPUBufferMapState map_state() const;

    void unmap();
};

}
