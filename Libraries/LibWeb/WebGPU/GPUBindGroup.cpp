/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUBindGroup.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUBindGroup);

GPUBindGroup::GPUBindGroup(JS::Realm& realm, NativeGPUBindGroup native_gpu_bind_group)
    : PlatformObject(realm)
    , m_native_gpu_bind_group(move(native_gpu_bind_group))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUBindGroup>> GPUBindGroup::create(JS::Realm& realm, NativeGPUBindGroup native_gpu_bind_group)
{
    return realm.create<GPUBindGroup>(realm, move(native_gpu_bind_group));
}

void GPUBindGroup::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUBindGroup);
    Base::initialize(realm);
}

void GPUBindGroup::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUBindGroup::label() const
{
    return m_native_gpu_bind_group.label();
}

void GPUBindGroup::set_label(String const& label)
{
    m_native_gpu_bind_group.set_label(label);
}

}
