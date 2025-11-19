/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUQueuePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>
#include <LibWeb/WebGPU/Native/NativeGPUQueue.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

class GPUQueue final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUQueue, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUQueue);

    static JS::ThrowCompletionOr<GC::Ref<GPUQueue>> create(JS::Realm&, NativeGPUQueue);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUQueue& native_gpu_queue() { return m_native_gpu_queue; }

    void submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers);

    void write_buffer(GC::Root<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, Optional<WebIDL::UnsignedLongLong> data_offset = 0, Optional<WebIDL::UnsignedLongLong> size = {});

private:
    explicit GPUQueue(JS::Realm&, NativeGPUQueue);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUQueue m_native_gpu_queue;
};

}
