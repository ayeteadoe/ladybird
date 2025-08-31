/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebGPU/GPUShaderModule.h>
#include <LibWeb/WebGPU/GPUTexture.h>

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
    wgpu::Instance instance { nullptr };
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

JS::ThrowCompletionOr<GC::Ref<GPUDevice>> GPUDevice::create(JS::Realm& realm, wgpu::Instance instance, wgpu::Device device, String const& label)
{
    auto queue = device.GetQueue();
    return realm.create<GPUDevice>(realm, Impl { .device = move(device), .label = label, .queue = TRY(GPUQueue::create(realm, instance, move(queue))) });
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

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createtexture
// FIXME: Spec comments
GC::Ref<GPUTexture> GPUDevice::create_texture(GPUTextureDescriptor const& options) const
{
    wgpu::TextureDescriptor texture_descriptor_options = options.to_wgpu();
    wgpu::Texture native_texture = m_impl->device.CreateTexture(&texture_descriptor_options);
    auto& realm = this->realm();
    return MUST(GPUTexture::create(realm, move(native_texture)));
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createshadermodule
// FIXME: Spec comments
GC::Ref<GPUShaderModule> GPUDevice::create_shader_module(GPUShaderModuleDescriptor const& options) const
{
    wgpu::ShaderSourceWGSL wgsl_source;
    auto code_view = options.code.bytes_as_string_view();
    auto code_view_bytes = code_view.bytes();
    wgsl_source.code = wgpu::StringView { bit_cast<char const*>(code_view_bytes.data()), code_view_bytes.size() };

    wgpu::ShaderModuleDescriptor shader_module_descriptor_options { .nextInChain = &wgsl_source };

    wgpu::ShaderModule native_shader_module = m_impl->device.CreateShaderModule(&shader_module_descriptor_options);

    auto& realm = this->realm();
    return MUST(GPUShaderModule::create(realm, move(native_shader_module)));
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
