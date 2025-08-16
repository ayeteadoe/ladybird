/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUShaderModule.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUShaderModule);

struct GPUShaderModule::Impl {
    wgpu::ShaderModule shader_module = { nullptr };
    String label;
};

GPUShaderModule::GPUShaderModule(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUShaderModule::~GPUShaderModule() = default;

JS::ThrowCompletionOr<GC::Ref<GPUShaderModule>> GPUShaderModule::create(JS::Realm& realm, wgpu::ShaderModule shader_module)
{
    return realm.create<GPUShaderModule>(realm, Impl { .shader_module = move(shader_module) });
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

wgpu::ShaderModule GPUShaderModule::as_wgpu() const
{
    return m_impl->shader_module;
}

String const& GPUShaderModule::label() const
{
    return m_impl->label;
}

void GPUShaderModule::set_label(String const& label)
{
    m_impl->label = label;
}

}
