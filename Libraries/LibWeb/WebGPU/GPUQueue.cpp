/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/ArrayBuffer.h>
#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/HTML/Scripting/TemporaryExecutionContext.h>
#include <LibWeb/Platform/EventLoopPlugin.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>
#include <LibWeb/WebGPU/GPUQueue.h>
#include <LibWeb/WebIDL/Buffers.h>
#include <LibWeb/WebIDL/Promise.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUQueue);

wgpu::QueueDescriptor GPUQueueDescriptor::to_wgpu() const
{
    auto const label_view = label.bytes_as_string_view();
    auto const label_byte_string = label_view.to_byte_string();
    return wgpu::QueueDescriptor { .nextInChain = nullptr, .label = wgpu::StringView { label_byte_string.characters(), label_byte_string.length() } };
}

struct GPUQueue::Impl {
    wgpu::Instance instance { nullptr };
    wgpu::Queue queue { nullptr };
    String label;
};

GPUQueue::GPUQueue(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUQueue::~GPUQueue() = default;

JS::ThrowCompletionOr<GC::Ref<GPUQueue>> GPUQueue::create(JS::Realm& realm, wgpu::Instance instance, wgpu::Queue queue)
{
    return realm.create<GPUQueue>(realm, Impl { .instance = instance, .queue = move(queue), .label = ""_string });
}

void GPUQueue::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUQueue);
    Base::initialize(realm);
}

void GPUQueue::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::Queue GPUQueue::wgpu() const
{
    return m_impl->queue;
}

String const& GPUQueue::label() const
{
    return m_impl->label;
}

void GPUQueue::set_label(String const& label)
{
    m_impl->label = label;
}

// https://www.w3.org/TR/webgpu/#dom-gpuqueue-submit
// FIXME: Spec comments
void GPUQueue::submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers)
{
    Vector<wgpu::CommandBuffer> wgpu_command_buffers;
    for (auto const& command_buffer : command_buffers) {
        wgpu_command_buffers.append(command_buffer->as_wgpu());
    }
    m_impl->queue.Submit(command_buffers.size(), wgpu_command_buffers.data());
}

// https://www.w3.org/TR/webgpu/#dom-gpuqueue-onsubmittedworkdone
// FIXME: Spec comments
GC::Ref<WebIDL::Promise> GPUQueue::on_submitted_work_done()
{
    auto& realm = this->realm();
    GC::Ref promise = WebIDL::create_promise(realm);
    Platform::EventLoopPlugin::the().deferred_invoke(GC::create_function(realm.heap(), [this, &realm, promise]() mutable {
        m_impl->instance.WaitAny(m_impl->queue.OnSubmittedWorkDone(wgpu::CallbackMode::AllowProcessEvents, [realm = GC::Root(realm), promise = GC::Root(promise)](wgpu::QueueWorkDoneStatus status, wgpu::StringView message) {
            if (status == wgpu::QueueWorkDoneStatus::Success) {
                HTML::TemporaryExecutionContext const context { *realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::resolve_promise(*realm, *promise, JS::js_undefined());
            } else {
                HTML::TemporaryExecutionContext const context { *realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::reject_promise(*realm, *promise, WebIDL::OperationError::create(*realm, Utf16String::formatted("Unable to complete submitted queue work: {}"sv, StringView { message.data, message.length })));
            }
        }),
            UINT64_MAX);
    }));

    return promise;
}

void GPUQueue::write_buffer(GC::Ref<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, /*FIXME: Dawn does not expose this field*/ [[maybe_unused]] Optional<WebIDL::UnsignedLongLong> data_offset, Optional<WebIDL::UnsignedLongLong> size)
{
    ByteBuffer const data_buffer = data->viewed_array_buffer()->buffer();
    m_impl->queue.WriteBuffer(buffer->as_wgpu(), buffer_offset, data_buffer.data(), size.value_or(0));
    dbgln("Wrote buffer of size {} to the device", size);
}

}
