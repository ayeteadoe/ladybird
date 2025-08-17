/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>
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
    GC::Ref<GPU> instance;
    GC::Ref<GPUQueue> queue;
};

GPUDevice::GPUDevice(JS::Realm& realm, Impl impl)
    : EventTarget(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUDevice::~GPUDevice() = default;

JS::ThrowCompletionOr<GC::Ref<GPUDevice>> GPUDevice::create(JS::Realm& realm, GPU& instance, wgpu::Device device, String const& label)
{
    auto queue = device.GetQueue();
    return realm.create<GPUDevice>(realm, Impl { .device = move(device), .label = label, .instance = instance, .queue = TRY(GPUQueue::create(realm, instance, move(queue))) });
}

void GPUDevice::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUDevice);
    EventTarget::initialize(realm);
}

void GPUDevice::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_impl->instance);
    visitor.visit(m_impl->queue);
}

wgpu::Device GPUDevice::wgpu() const
{
    return m_impl->device;
}

// NOTE: wgpu::Device does not have a GetLabel() method exposed

String const& GPUDevice::label() const
{
    return m_impl->label;
}

void GPUDevice::set_label(String const& label)
{
    m_impl->label = label;
    auto label_view = label.bytes_as_string_view();
    m_impl->device.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

GC::Ref<GPU> GPUDevice::instance() const
{
    return m_impl->instance;
}

GC::Ref<GPUQueue> GPUDevice::queue() const
{
    return m_impl->queue;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createcommandencoder
// FIXME: Spec comments
GC::Ref<GPUCommandEncoder> GPUDevice::create_command_encoder(GPUCommandEncoderDescriptor const& options) const
{
    wgpu::CommandEncoderDescriptor command_encoder_descriptor_options = options.to_wgpu();
    wgpu::CommandEncoder native_command_encoder = m_impl->device.CreateCommandEncoder(&command_encoder_descriptor_options);
    auto& realm = this->realm();
    return MUST(GPUCommandEncoder::create(realm, move(native_command_encoder)));
}

}
