/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUBindGroupLayout.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUBindGroupLayout);

GPUBindGroupLayout::GPUBindGroupLayout(JS::Realm& realm, NativeGPUBindGroupLayout native_gpu_bind_group_layout)
    : PlatformObject(realm)
    , m_native_gpu_bind_group_layout(move(native_gpu_bind_group_layout))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUBindGroupLayout>> GPUBindGroupLayout::create(JS::Realm& realm, NativeGPUBindGroupLayout native_gpu_bind_group_layout)
{
    return realm.create<GPUBindGroupLayout>(realm, move(native_gpu_bind_group_layout));
}

void GPUBindGroupLayout::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUBindGroupLayout);
    Base::initialize(realm);
}

void GPUBindGroupLayout::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUBindGroupLayout::label() const
{
    return m_native_gpu_bind_group_layout.label();
}

void GPUBindGroupLayout::set_label(String const& label)
{
    m_native_gpu_bind_group_layout.set_label(label);
}

}
