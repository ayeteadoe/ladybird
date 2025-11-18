/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUBindGroupLayoutPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUBindGroupLayout.h>

namespace Web::WebGPU {

class GPUBindGroupLayout final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUBindGroupLayout, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUBindGroupLayout);

    static JS::ThrowCompletionOr<GC::Ref<GPUBindGroupLayout>> create(JS::Realm&, NativeGPUBindGroupLayout);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUBindGroupLayout& native_gpu_bind_group_layout() { return m_native_gpu_bind_group_layout; }

private:
    explicit GPUBindGroupLayout(JS::Realm&, NativeGPUBindGroupLayout);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUBindGroupLayout m_native_gpu_bind_group_layout;
};

}
