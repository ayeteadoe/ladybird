/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebGPU/GPUTexture.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUDevice);

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

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createtexture
// FIXME: Spec comments
GC::Ref<GPUTexture> GPUDevice::create_texture(GPUTextureDescriptor const& options) const
{
    auto to_view_format = [](Bindings::GPUTextureFormat format) -> Optional<wgpu::TextureFormat> {
        // FIXME: Support remaining texture formats
        switch (format) {
        case Bindings::GPUTextureFormat::Bgra8unorm:
            return wgpu::TextureFormat::BGRA8Unorm;
        default:
            return {};
        }
    };
    Vector<wgpu::TextureFormat> texture_formats;
    for (auto const& view_format : options.view_formats) {
        auto const texture_format = to_view_format(view_format);
        if (texture_format.has_value()) {
            texture_formats.append(texture_format.value());
        }
    }

    wgpu::Extent3D extent { .width = options.size.width, .height = options.size.height, .depthOrArrayLayers = options.size.depth_or_array_layers };
    wgpu::TextureDescriptor texture_descriptor { .size = extent, .mipLevelCount = options.mip_level_count, .sampleCount = options.sample_count, .viewFormatCount = texture_formats.size(), .viewFormats = texture_formats.data() };
    auto const wgpu_format = to_view_format(options.format);
    if (wgpu_format.has_value()) {
        texture_descriptor.format = wgpu_format.value();
    }
    switch (options.dimension) {
    case Bindings::GPUTextureDimension::_1d: {
        texture_descriptor.dimension = wgpu::TextureDimension::e1D;
        break;
    }
    case Bindings::GPUTextureDimension::_2d: {
        texture_descriptor.dimension = wgpu::TextureDimension::e2D;
        break;
    }
    case Bindings::GPUTextureDimension::_3d: {
        texture_descriptor.dimension = wgpu::TextureDimension::e3D;
        break;
    }
    default:
        break;
    }
    wgpu::TextureUsage texture_usage = wgpu::TextureUsage::None;
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::CopySrc)) {
        texture_usage |= wgpu::TextureUsage::CopySrc;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::CopyDst)) {
        texture_usage |= wgpu::TextureUsage::CopyDst;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::TextureBinding)) {
        texture_usage |= wgpu::TextureUsage::TextureBinding;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::StorageBinding)) {
        texture_usage |= wgpu::TextureUsage::StorageBinding;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::RenderAttachment)) {
        texture_usage |= wgpu::TextureUsage::RenderAttachment;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::TransientAttachment)) {
        texture_usage |= wgpu::TextureUsage::TransientAttachment;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::StorageAttachment)) {
        texture_usage |= wgpu::TextureUsage::StorageAttachment;
    }
    texture_descriptor.usage = texture_usage;
    wgpu::Texture native_texture = m_impl->device.CreateTexture(&texture_descriptor);
    auto& realm = this->realm();
    return MUST(GPUTexture::create(realm, move(native_texture)));
}

}
