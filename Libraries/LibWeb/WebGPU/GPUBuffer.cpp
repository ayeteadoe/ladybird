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
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebIDL/Promise.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUBuffer);

wgpu::BufferDescriptor GPUBufferDescriptor::to_wgpu() const
{
    // FIXME: Support all buffer usage flags
    wgpu::BufferUsage buffer_usage {};
    auto const map_read = static_cast<bool>(usage & static_cast<u32>(wgpu::BufferUsage::MapRead));
    auto const map_write = static_cast<bool>(usage & static_cast<u32>(wgpu::BufferUsage::MapWrite));
    auto const copy_src = static_cast<bool>(usage & static_cast<u32>(wgpu::BufferUsage::CopySrc));
    auto const copy_dest = static_cast<bool>(usage & static_cast<u32>(wgpu::BufferUsage::CopyDst));
    auto const vertex = static_cast<bool>(usage & static_cast<u32>(wgpu::BufferUsage::Vertex));
    if (map_read) {
        buffer_usage |= wgpu::BufferUsage::MapRead;
    }
    if (map_write) {
        buffer_usage |= wgpu::BufferUsage::MapWrite;
    }
    if (copy_src) {
        buffer_usage |= wgpu::BufferUsage::CopySrc;
    }
    if (copy_dest) {
        buffer_usage |= wgpu::BufferUsage::CopyDst;
    }
    if (vertex) {
        buffer_usage |= wgpu::BufferUsage::Vertex;
    }

    return wgpu::BufferDescriptor { .size = size, .usage = buffer_usage, .mappedAtCreation = mapped_at_creation };
}

struct GPUBuffer::Impl {
    wgpu::Instance instance { nullptr };
    wgpu::Buffer buffer = { nullptr };
    String label;
};

GPUBuffer::GPUBuffer(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUBuffer::~GPUBuffer() = default;

JS::ThrowCompletionOr<GC::Ref<GPUBuffer>> GPUBuffer::create(JS::Realm& realm, wgpu::Instance instance, wgpu::Buffer buffer)
{
    return realm.create<GPUBuffer>(realm, Impl { .instance = instance, .buffer = move(buffer) });
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
        m_impl->instance.WaitAny(m_impl->buffer.MapAsync(map_mode, offset.value_or(0), size.value_or(0), wgpu::CallbackMode::AllowProcessEvents, [this, realm = GC::Root(realm), promise = GC::Root(promise)](wgpu::MapAsyncStatus status, wgpu::StringView message) {
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
