/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUTextureView.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUTextureView);

GPUTextureView::GPUTextureView(JS::Realm& realm, NativeGPUTextureView native_gpu_texture_view)
    : PlatformObject(realm)
    , m_native_gpu_texture_view(move(native_gpu_texture_view))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUTextureView>> GPUTextureView::create(JS::Realm& realm, NativeGPUTextureView native_gpu_texture_view)
{
    return realm.create<GPUTextureView>(realm, move(native_gpu_texture_view));
}

void GPUTextureView::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUTextureView);
    Base::initialize(realm);
}

void GPUTextureView::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUTextureView::label() const
{
    return m_native_gpu_texture_view.label();
}

void GPUTextureView::set_label(String const& label)
{
    m_native_gpu_texture_view.set_label(label);
}

}
