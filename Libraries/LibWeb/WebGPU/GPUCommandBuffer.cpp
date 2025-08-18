/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUCommandBuffer);

wgpu::CommandBufferDescriptor GPUCommandBufferDescriptor::to_wgpu() const
{
    return wgpu::CommandBufferDescriptor {};
}

struct GPUCommandBuffer::Impl {
    wgpu::CommandBuffer command_buffer = { nullptr };
    String label;
};

GPUCommandBuffer::GPUCommandBuffer(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUCommandBuffer::~GPUCommandBuffer() = default;

JS::ThrowCompletionOr<GC::Ref<GPUCommandBuffer>> GPUCommandBuffer::create(JS::Realm& realm, wgpu::CommandBuffer command_buffer)
{
    return realm.create<GPUCommandBuffer>(realm, Impl { .command_buffer = move(command_buffer), .label = ""_string });
}

void GPUCommandBuffer::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUCommandBuffer);
    Base::initialize(realm);
}

void GPUCommandBuffer::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::CommandBuffer GPUCommandBuffer::as_wgpu() const
{
    return m_impl->command_buffer;
}

String const& GPUCommandBuffer::label() const
{
    return m_impl->label;
}

void GPUCommandBuffer::set_label(String const& label)
{
    m_impl->label = label;
}

}
