/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUBufferPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class Buffer;
struct BufferDescriptor;

}

namespace Web::WebGPU {

struct GPUBufferDescriptor : GPUObjectDescriptorBase {
    u64 size;
    u32 usage;
    bool mapped_at_creation = { false };
};

class GPUBuffer final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUBuffer, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUBuffer);

    static JS::ThrowCompletionOr<GC::Ref<GPUBuffer>> create(JS::Realm&, GPU&, wgpu::Buffer);

    ~GPUBuffer() override;

    String const& label() const;
    void set_label(String const& label);

    u64 size() const;
    u32 usage() const;
    Bindings::GPUBufferMapState map_state() const;

private:
    struct Impl;
    explicit GPUBuffer(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
