/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/GPUAdapter.h>

namespace Web::WebGPU {

class GPUAdapter : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUAdapter, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUAdapter);

public:
    [[nodiscard]] static GC::Ref<GPUAdapter> create(JS::Realm&, Native::GPUAdapter native_gpu_adapter);

    virtual ~GPUAdapter() override;

    // GC::Ref<WebIDL::Promise> request_adapter();

private:
    explicit GPUAdapter(JS::Realm&, Native::GPUAdapter native_gpu_adapter);
};

}
