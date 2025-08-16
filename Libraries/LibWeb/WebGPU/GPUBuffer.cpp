/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteBuffer.h>
#include <LibJS/Runtime/ArrayBuffer.h>
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
};

GPUBuffer::GPUBuffer(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUBuffer::~GPUBuffer() = default;

JS::ThrowCompletionOr<GC::Ref<GPUBuffer>> GPUBuffer::create(JS::Realm& realm, GPU& instance, wgpu::Buffer buffer)
{
    return realm.create<GPUBuffer>(realm, Impl { .buffer = move(buffer), .label = ""_string, .instance = instance });
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

wgpu::Buffer GPUBuffer::as_wgpu() const
{
    return m_impl->buffer;
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
    return m_impl->buffer.GetSize();
}

u32 GPUBuffer::usage() const
{
    return static_cast<u32>(m_impl->buffer.GetUsage());
}

Bindings::GPUBufferMapState GPUBuffer::map_state() const
{
    Bindings::GPUBufferMapState map_state { 0 };
    switch (m_impl->buffer.GetMapState()) {
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
    return map_state;
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

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-getmappedrange
// FIXME: Spec comments
WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> GPUBuffer::get_mapped_range(Optional<WebIDL::UnsignedLongLong> offset, Optional<WebIDL::UnsignedLongLong> size)
{
    auto& realm = this->realm();
    auto const mapped_range_size = size.value_or(wgpu::kWholeMapSize);

    // https://github.com/webgpu-native/webgpu-headers/issues/194
    // FIXME: Support MAP_WRITE Buffer usage
    VERIFY(!(m_impl->buffer.GetUsage() & wgpu::BufferUsage::MapWrite));
    auto mapped_range = m_impl->buffer.GetConstMappedRange(offset.value_or(0), mapped_range_size);
    if (mapped_range == nullptr)
        return realm.vm().throw_completion<JS::InternalError>("Unable to retrieve mapped range"_string);
    auto mapped_byte_buffer = MUST(ByteBuffer::copy(mapped_range, mapped_range_size));

    return JS::ArrayBuffer::create(realm, move(mapped_byte_buffer));
}

// https://www.w3.org/TR/webgpu/#dom-gpubuffer-unmap
// FIXME: Spec comments
void GPUBuffer::unmap()
{
    m_impl->buffer.Unmap();
}

}
