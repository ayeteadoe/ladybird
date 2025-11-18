/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUShaderModule.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUShaderModule);

GPUShaderModule::GPUShaderModule(JS::Realm& realm, NativeGPUShaderModule native_gpu_shader_module)
    : PlatformObject(realm)
    , m_native_gpu_shader_module(move(native_gpu_shader_module))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUShaderModule>> GPUShaderModule::create(JS::Realm& realm, NativeGPUShaderModule native_gpu_shader_module)
{
    return realm.create<GPUShaderModule>(realm, move(native_gpu_shader_module));
}

void GPUShaderModule::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUShaderModule);
    Base::initialize(realm);
}

void GPUShaderModule::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUShaderModule::label() const
{
    return m_native_gpu_shader_module.label();
}

void GPUShaderModule::set_label(String const& label)
{
    m_native_gpu_shader_module.set_label(label);
}

}
