/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUTextureViewPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUTextureView.h>

namespace Web::WebGPU {

class GPUTextureView final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUTextureView, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUTextureView);

    static JS::ThrowCompletionOr<GC::Ref<GPUTextureView>> create(JS::Realm&, NativeGPUTextureView);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUTextureView& native_gpu_texture_view() { return m_native_gpu_texture_view; }

private:
    explicit GPUTextureView(JS::Realm&, NativeGPUTextureView);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUTextureView m_native_gpu_texture_view;
};

}
