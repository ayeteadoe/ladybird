/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUShaderModulePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUShaderModule.h>

namespace Web::WebGPU {

class GPUShaderModule final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUShaderModule, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUShaderModule);

    static JS::ThrowCompletionOr<GC::Ref<GPUShaderModule>> create(JS::Realm&, NativeGPUShaderModule);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUShaderModule& native_gpu_shader_module() { return m_native_gpu_shader_module; }

private:
    explicit GPUShaderModule(JS::Realm&, NativeGPUShaderModule);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUShaderModule m_native_gpu_shader_module;
};

}
