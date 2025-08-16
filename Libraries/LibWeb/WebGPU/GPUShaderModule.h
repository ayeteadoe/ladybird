/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUShaderModulePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class ShaderModule;
struct ShaderModuleDescriptor;

}

namespace Web::WebGPU {

struct GPUShaderModuleDescriptor : GPUObjectDescriptorBase {
    String code;
};

class GPUShaderModule final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUShaderModule, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUShaderModule);

    static JS::ThrowCompletionOr<GC::Ref<GPUShaderModule>> create(JS::Realm&, wgpu::ShaderModule);

    ~GPUShaderModule() override;

    wgpu::ShaderModule as_wgpu() const;

    String const& label() const;
    void set_label(String const& label);

private:
    struct Impl;
    explicit GPUShaderModule(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
