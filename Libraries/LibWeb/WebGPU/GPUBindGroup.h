/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUBindGroupPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUBindGroup.h>

namespace Web::WebGPU {

class GPUBindGroup final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUBindGroup, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUBindGroup);

    static JS::ThrowCompletionOr<GC::Ref<GPUBindGroup>> create(JS::Realm&, NativeGPUBindGroup);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUBindGroup& native_gpu_bind_group() { return m_native_gpu_bind_group; }

private:
    explicit GPUBindGroup(JS::Realm&, NativeGPUBindGroup);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUBindGroup m_native_gpu_bind_group;
};

}
