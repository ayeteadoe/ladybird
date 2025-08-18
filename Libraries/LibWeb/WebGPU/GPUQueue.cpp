/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebGPU/GPUCommandBuffer.h>
#include <LibWeb/WebGPU/GPUQueue.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUQueue);

struct GPUQueue::Impl {
    wgpu::Queue queue { nullptr };
    String label;
    GC::Ref<GPU> instance;
};

GPUQueue::GPUQueue(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUQueue::~GPUQueue() = default;

JS::ThrowCompletionOr<GC::Ref<GPUQueue>> GPUQueue::create(JS::Realm& realm, GPU& instance, wgpu::Queue queue)
{
    return realm.create<GPUQueue>(realm, Impl { .queue = move(queue), .label = ""_string, .instance = instance });
}

void GPUQueue::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUQueue);
    Base::initialize(realm);
}

void GPUQueue::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_impl->instance);
}

wgpu::Queue GPUQueue::wgpu() const
{
    return m_impl->queue;
}

// NOTE: wgpu::Queue does not have a GetLabel() method exposed
String const& GPUQueue::label() const
{
    return m_impl->label;
}

void GPUQueue::set_label(String const& label)
{
    m_impl->label = label;
    auto label_view = label.bytes_as_string_view();
    m_impl->queue.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
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

}
