/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUDeviceLostInfo.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUDeviceLostInfo);

GPUDeviceLostInfo::GPUDeviceLostInfo(JS::Realm& realm, Bindings::GPUDeviceLostReason reason, String const& message)
    : PlatformObject(realm)
    , m_reason(reason)
    , m_message(message)
{
}

JS::ThrowCompletionOr<GC::Ref<GPUDeviceLostInfo>> GPUDeviceLostInfo::create(JS::Realm& realm, Bindings::GPUDeviceLostReason reason, String const& message)
{
    return realm.create<GPUDeviceLostInfo>(realm, reason, message);
}

void GPUDeviceLostInfo::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUDeviceLostInfo);
    Base::initialize(realm);
}

void GPUDeviceLostInfo::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

}
