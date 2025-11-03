/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUTexturePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUTexture.h>

namespace Web::WebGPU {

class GPUTexture final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUTexture, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUTexture);

    static JS::ThrowCompletionOr<GC::Ref<GPUTexture>> create(JS::Realm&, NativeGPUTexture);

    NativeGPUTexture& native_gpu_texture() { return m_native_gpu_texture; }

private:
    explicit GPUTexture(JS::Realm&, NativeGPUTexture);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUTexture m_native_gpu_texture;
};

}
