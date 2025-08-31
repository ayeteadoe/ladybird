/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/HTML/Scripting/TemporaryExecutionContext.h>
#include <LibWeb/Platform/EventLoopPlugin.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebIDL/Promise.h>

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

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-mapasync
// FIXME: Spec comments
GC::Ref<WebIDL::Promise> GPUBuffer::map_async(/*FIXME: Usage proper GPUMapModeFlags namespace*/ WebIDL::UnsignedLong mode, Optional<WebIDL::UnsignedLongLong> offset, Optional<WebIDL::UnsignedLongLong> size)
{
    wgpu::MapMode map_mode = wgpu::MapMode::None;
    if (mode & static_cast<WebIDL::UnsignedLong>(wgpu::MapMode::Read))
        map_mode |= wgpu::MapMode::Read;
    if (mode & static_cast<WebIDL::UnsignedLong>(wgpu::MapMode::Write))
        map_mode |= wgpu::MapMode::Write;
    auto& realm = this->realm();
    GC::Ref promise = WebIDL::create_promise(realm);
    Platform::EventLoopPlugin::the().deferred_invoke(GC::create_function(realm.heap(), [this, map_mode, offset, size, &realm, promise]() mutable {
        m_impl->instance->wgpu().WaitAny(m_impl->buffer.MapAsync(map_mode, offset.value_or(0), size.value_or(0), wgpu::CallbackMode::AllowProcessEvents, [this, realm = GC::Root(realm), promise = GC::Root(promise)](wgpu::MapAsyncStatus status, wgpu::StringView message) {
            auto& gpu_buffer_realm = HTML::relevant_realm(*this);
            if (status == wgpu::MapAsyncStatus::Success) {
                HTML::TemporaryExecutionContext const context { gpu_buffer_realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::resolve_promise(gpu_buffer_realm, *promise, JS::js_undefined());
            } else {
                HTML::TemporaryExecutionContext const context { gpu_buffer_realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::reject_promise(gpu_buffer_realm, *promise, WebIDL::OperationError::create(gpu_buffer_realm, Utf16String::formatted("Unable to map buffer: {}"sv, StringView { message.data, message.length })));
            }
        }),
            UINT64_MAX);
    }));

    return promise;
}

}
