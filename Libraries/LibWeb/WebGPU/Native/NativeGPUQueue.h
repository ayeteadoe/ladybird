/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUQueuePrototype.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

struct GPUQueueDescriptor : GPUObjectDescriptorBase {
};

class NativeGPUDevice;

class NativeGPUQueue {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUQueue);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUQueue);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUQueue);

public:
    friend NativeGPUDevice;

    static NativeGPUQueue create();

    void submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers);

    void write_buffer(GC::Root<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, Optional<WebIDL::UnsignedLongLong> data_offset, Optional<WebIDL::UnsignedLongLong> size);
};

}
