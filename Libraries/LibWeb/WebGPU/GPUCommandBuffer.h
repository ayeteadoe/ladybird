/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUCommandBufferPrototype.h>
#include <LibWeb/Bindings/GPUPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class CommandBuffer;
struct CommandBufferDescriptor;

}

namespace Web::WebGPU {

using GPUTextureUsageFlags = u32;

struct GPUCommandBufferDescriptor : GPUObjectDescriptorBase {
    wgpu::CommandBufferDescriptor to_wgpu() const;
};

class GPUCommandBuffer final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCommandBuffer, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCommandBuffer);

    static JS::ThrowCompletionOr<GC::Ref<GPUCommandBuffer>> create(JS::Realm&, wgpu::CommandBuffer);

    ~GPUCommandBuffer() override;

    wgpu::CommandBuffer as_wgpu() const;

    String const& label() const;
    void set_label(String const& label);

private:
    struct Impl;
    explicit GPUCommandBuffer(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
