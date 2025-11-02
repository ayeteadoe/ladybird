/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUAdapter.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUAdapter);

GPUAdapter::GPUAdapter(JS::Realm& realm, NativeGPUAdapter adapter)
    : PlatformObject(realm)
    , m_native_gpu_adapter(move(adapter))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUAdapter>> GPUAdapter::create(JS::Realm& realm, NativeGPUAdapter adapter)
{
    return realm.create<GPUAdapter>(realm, move(adapter));
}

void GPUAdapter::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUAdapter);
    Base::initialize(realm);
}

void GPUAdapter::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

// https://www.w3.org/TR/webgpu/#dom-gpuadapter-requestdevice
GC::Ref<WebIDL::Promise> GPUAdapter::request_device(Optional<GPUDeviceDescriptor> descriptor)
{
    // 1. Let contentTimeline be the current Content timeline.

    // 2. Let promise be a new promise.
    auto& realm = this->realm();
    GC::Ref promise = WebIDL::create_promise(realm);

    // 3. Let adapter be this.[[adapter]].
    auto& adapter = m_native_gpu_adapter;

    // 4. Issue the initialization steps to the Device timeline of this.
    adapter.request_device_initialization_steps(realm, promise, move(descriptor));

    // 5. Return promise.
    return promise;
}

}
