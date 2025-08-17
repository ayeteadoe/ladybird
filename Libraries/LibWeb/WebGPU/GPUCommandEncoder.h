/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUCommandEncoderPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class CommandEncoder;
struct CommandEncoderDescriptor;

}

namespace Web::WebGPU {

struct GPUCommandEncoderDescriptor : GPUObjectDescriptorBase {
    wgpu::CommandEncoderDescriptor to_wgpu() const;
};

class GPUCommandEncoder final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCommandEncoder, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCommandEncoder);

    static JS::ThrowCompletionOr<GC::Ref<GPUCommandEncoder>> create(JS::Realm&, wgpu::CommandEncoder);

    ~GPUCommandEncoder() override;

    wgpu::CommandEncoder as_wgpu() const;

    String const& label() const;
    void set_label(String const& label);

private:
    struct Impl;
    explicit GPUCommandEncoder(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
