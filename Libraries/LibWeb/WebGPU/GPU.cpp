/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPU);

GC::Ref<GPU> GPU::create(JS::Realm& realm)
{
    return realm.create<GPU>(realm, make<Native::GPU>());
}

GPU::GPU(JS::Realm& realm, [[maybe_unused]] NonnullOwnPtr<Native::GPU> native_gpu)
    : PlatformObject(realm)
{
}

GPU::~GPU() = default;

GC::Ref<WebIDL::Promise> GPU::request_adapter()
{

    auto p = WebIDL::create_promise(realm());
    return p;
}

}
