/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUTexture.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUTexture);

GPUTexture::GPUTexture(JS::Realm& realm, NativeGPUTexture native_gpu_texture)
    : PlatformObject(realm)
    , m_native_gpu_texture(move(native_gpu_texture))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUTexture>> GPUTexture::create(JS::Realm& realm, NativeGPUTexture native_gpu_texture)
{
    return realm.create<GPUTexture>(realm, move(native_gpu_texture));
}

void GPUTexture::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUTexture);
    Base::initialize(realm);
}

void GPUTexture::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

}
