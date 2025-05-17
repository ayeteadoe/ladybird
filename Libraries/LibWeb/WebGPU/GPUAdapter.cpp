/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/GPUAdapter.h>

#include <LibJS/Runtime/Realm.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUAdapter);

GC::Ref<GPUAdapter> GPUAdapter::create(JS::Realm& realm, Native::GPUAdapter native_gpu_adapter)
{
    return realm.create<GPUAdapter>(realm, std::move(native_gpu_adapter));
}

GPUAdapter::GPUAdapter(JS::Realm& realm, [[maybe_unused]] Native::GPUAdapter native_gpu_adapter)
    : PlatformObject(realm)
{
}

GPUAdapter::~GPUAdapter() = default;

}
