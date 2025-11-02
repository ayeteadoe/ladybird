/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPU);

GPU::GPU(JS::Realm& realm)
    : PlatformObject(realm)
    , m_native_gpu(NativeGPU::create())
{
}

void GPU::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPU);
    Base::initialize(realm);
}

void GPU::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

// https://gpuweb.github.io/gpuweb/#dom-gpu-requestadapter
GC::Ref<WebIDL::Promise> GPU::request_adapter(Optional<GPURequestAdapterOptions> options)
{
    // 1. Let contentTimeline be the current Content timeline.

    // 2. Let promise be a new promise.
    auto& realm = this->realm();
    GC::Ref promise = WebIDL::create_promise(realm);

    // 3. Issue the initialization steps on the Device timeline of this.
    m_native_gpu.request_adapter_initialization_steps(realm, promise, move(options));

    // 4. Return promise.
    return promise;
}

}
