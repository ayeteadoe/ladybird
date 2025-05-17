/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/GPU.h>

namespace Web::WebGPU {

class GPU : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPU, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPU);

public:
    [[nodiscard]] static GC::Ref<GPU> create(JS::Realm&);

    virtual ~GPU() override;

    GC::Ref<WebIDL::Promise> request_adapter();

private:
    explicit GPU(JS::Realm&, NonnullOwnPtr<Native::GPU> native_gpu);
};

}
