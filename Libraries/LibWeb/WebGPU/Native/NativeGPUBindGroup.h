/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUBindGroupPrototype.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

struct GPUBufferBinding {
    GC::Root<GPUBuffer> buffer;
    WebIDL::UnsignedLongLong offset = 0;
    Optional<WebIDL::UnsignedLongLong> size;
};

using GPUBindingResource = Variant<GC::Root<GPUTexture>, GC::Root<GPUTextureView>, GC::Root<GPUBuffer>, GPUBufferBinding, Empty>;

struct GPUBindGroupEntry {
    WebIDL::UnsignedLong binding;
    GPUBindingResource resource;
};

struct GPUBindGroupDescriptor : GPUObjectDescriptorBase {
    GC::Ptr<GPUBindGroupLayout> layout;
    Vector<GPUBindGroupEntry> entries;
};

class NativeGPUDevice;

class NativeGPUBindGroup {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUBindGroup);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUBindGroup);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUBindGroup);

public:
    friend NativeGPUDevice;

    static NativeGPUBindGroup create();
};

}
