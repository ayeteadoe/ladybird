/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCommandEncoder);

wgpu::CommandEncoderDescriptor GPUCommandEncoderDescriptor::to_wgpu() const
{
    return wgpu::CommandEncoderDescriptor {};
}

struct GPUCommandEncoder::Impl {
    wgpu::CommandEncoder command_encoder = { nullptr };
    String label;
};

GPUCommandEncoder::GPUCommandEncoder(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUCommandEncoder::~GPUCommandEncoder() = default;

JS::ThrowCompletionOr<GC::Ref<GPUCommandEncoder>> GPUCommandEncoder::create(JS::Realm& realm, wgpu::CommandEncoder command_encoder)
{
    return realm.create<GPUCommandEncoder>(realm, Impl { .command_encoder = move(command_encoder), .label = ""_string });
}

void GPUCommandEncoder::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCommandEncoder);
    Base::initialize(realm);
}

void GPUCommandEncoder::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::CommandEncoder GPUCommandEncoder::as_wgpu() const
{
    return m_impl->command_encoder;
}

String const& GPUCommandEncoder::label() const
{
    return m_impl->label;
}

void GPUCommandEncoder::set_label(String const& label)
{
    m_impl->label = label;
}

}
