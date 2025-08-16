/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUBuffer.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUBuffer);

struct GPUBuffer::Impl {
    wgpu::Buffer buffer = { nullptr };
    String label;
    GC::Ref<GPU> instance;
    u64 size;
    u32 usage;
    Bindings::GPUBufferMapState map_state;
};

GPUBuffer::GPUBuffer(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUBuffer::~GPUBuffer() = default;

JS::ThrowCompletionOr<GC::Ref<GPUBuffer>> GPUBuffer::create(JS::Realm& realm, GPU& instance, wgpu::Buffer buffer)
{
    u64 const size = buffer.GetSize();
    auto const usage = static_cast<u32>(buffer.GetUsage());
    Bindings::GPUBufferMapState map_state { 0 };
    switch (buffer.GetMapState()) {
    case wgpu::BufferMapState::Unmapped: {
        map_state = Bindings::GPUBufferMapState::Unmapped;
        break;
    }
    case wgpu::BufferMapState::Mapped: {
        map_state = Bindings::GPUBufferMapState::Mapped;
        break;
    }
    case wgpu::BufferMapState::Pending: {
        map_state = Bindings::GPUBufferMapState::Pending;
        break;
    }
    default:
        break;
    }

    return realm.create<GPUBuffer>(realm, Impl { .buffer = move(buffer), .label = ""_string, .instance = instance, .size = size, .usage = usage, .map_state = map_state });
}

void GPUBuffer::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUBuffer);
    Base::initialize(realm);
}

void GPUBuffer::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

String const& GPUBuffer::label() const
{
    return m_impl->label;
}

void GPUBuffer::set_label(String const& label)
{
    m_impl->label = label;
}

u64 GPUBuffer::size() const
{
    return m_impl->size;
}

u32 GPUBuffer::usage() const
{
    return m_impl->usage;
}

Bindings::GPUBufferMapState GPUBuffer::map_state() const
{
    return m_impl->map_state;
}

}
