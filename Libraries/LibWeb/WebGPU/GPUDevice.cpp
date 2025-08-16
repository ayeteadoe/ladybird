/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/GPUDevice.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUDevice);

wgpu::DeviceDescriptor GPUDeviceDescriptor::to_wgpu() const
{
    auto const label_view = label.bytes_as_string_view();
    auto const label_byte_string = label_view.to_byte_string();
    return wgpu::DeviceDescriptor { wgpu::DeviceDescriptor::Init { .nextInChain = nullptr, .label = wgpu::StringView { label_byte_string.characters(), label_byte_string.length() }, .defaultQueue = default_queue.to_wgpu() } };
}

struct GPUDevice::Impl {
    wgpu::Device device { nullptr };
    String label;
    GC::Ref<GPUQueue> queue;
};

GPUDevice::GPUDevice(JS::Realm& realm, Impl impl)
    : EventTarget(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUDevice::~GPUDevice() = default;

JS::ThrowCompletionOr<GC::Ref<GPUDevice>> GPUDevice::create(JS::Realm& realm, wgpu::Device device)
{
    auto queue = device.GetQueue();
    return realm.create<GPUDevice>(realm, Impl { .device = move(device), .label = ""_string, .queue = TRY(GPUQueue::create(realm, move(queue))) });
}

void GPUDevice::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUDevice);
    EventTarget::initialize(realm);
}

void GPUDevice::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_impl->queue);
}

String const& GPUDevice::label() const
{
    return m_impl->label;
}

void GPUDevice::set_label(String const& label)
{
    m_impl->label = label;
}

GC::Ref<GPUQueue> GPUDevice::queue() const
{
    return m_impl->queue;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createbuffer
GC::Ref<GPUBuffer> GPUDevice::create_buffer(GPUBufferDescriptor const& options) const
{
    wgpu::BufferDescriptor buffer_descriptor_options = options.to_wgpu();

    // Content timeline steps:
    // 1. Let b be ! create a new WebGPU object(this, GPUBuffer, descriptor).
    // 2. Set b.size to descriptor.size.
    // 3. Set b.usage to descriptor.usage.
    // 4. FIXME: If descriptor.mappedAtCreation is true:
    //      1. If descriptor.size is not a multiple of 4, throw a RangeError.
    //      2. Set b.[[mapping]] to ? initialize an active buffer mapping with mode WRITE and range [0, descriptor.size].
    if (buffer_descriptor_options.mappedAtCreation) {
    }

    // FIXME: Issue the initialization steps on the Device timeline of this.

    wgpu::Buffer native_buffer = m_impl->device.CreateBuffer(&buffer_descriptor_options);

    auto& realm = this->realm();
    return MUST(GPUBuffer::create(realm, move(native_buffer)));
}

}
