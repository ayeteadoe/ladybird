/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUQueuePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebIDL/Types.h>

namespace wgpu {

class Instance;
class Queue;
struct QueueDescriptor;

}

namespace Web::WebGPU {

struct GPUQueueDescriptor : GPUObjectDescriptorBase {
};

class GPUQueue final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUQueue, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUQueue);

    static JS::ThrowCompletionOr<GC::Ref<GPUQueue>> create(JS::Realm&, GPU&, wgpu::Queue);

    ~GPUQueue() override;

    wgpu::Queue wgpu() const;

    String const& label() const;
    void set_label(String const& label);

    void submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers);

    GC::Ref<WebIDL::Promise> on_submitted_work_done();

    void write_buffer(GC::Ref<GPUBuffer>, WebIDL::UnsignedLongLong, GC::Root<WebIDL::BufferSource> const&, Optional<WebIDL::UnsignedLongLong>, Optional<WebIDL::UnsignedLongLong>);

private:
    struct Impl;
    explicit GPUQueue(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

}
