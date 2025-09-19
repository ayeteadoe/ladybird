/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUTexture.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUTexture);

wgpu::TextureDescriptor GPUTextureDescriptor::to_wgpu() const
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
    Vector<wgpu::TextureFormat> wgpu_view_formats;
    for (auto const& view_format : view_formats) {
        auto const wgpu_view_format = to_view_format(view_format);
        if (wgpu_view_format.has_value()) {
            wgpu_view_formats.append(wgpu_view_format.value());
        }
    }

    wgpu::Extent3D extent { .width = size.width, .height = size.height, .depthOrArrayLayers = size.depth_or_array_layers };
    wgpu::TextureDescriptor wgpu_descriptor { .size = extent, .mipLevelCount = mip_level_count, .sampleCount = sample_count, .viewFormatCount = wgpu_view_formats.size(), .viewFormats = wgpu_view_formats.data() };
    auto const wgpu_format = to_view_format(format);
    if (wgpu_format.has_value()) {
        wgpu_descriptor.format = wgpu_format.value();
    }
    switch (dimension) {
    case Bindings::GPUTextureDimension::_1d: {
        wgpu_descriptor.dimension = wgpu::TextureDimension::e1D;
        break;
    }
    case Bindings::GPUTextureDimension::_2d: {
        wgpu_descriptor.dimension = wgpu::TextureDimension::e2D;
        break;
    }
    case Bindings::GPUTextureDimension::_3d: {
        wgpu_descriptor.dimension = wgpu::TextureDimension::e3D;
        break;
    }
    default:
        break;
    }
    wgpu::TextureUsage wgpu_usage = wgpu::TextureUsage::None;
    if (usage & static_cast<u32>(wgpu::TextureUsage::CopySrc)) {
        wgpu_usage |= wgpu::TextureUsage::CopySrc;
    }
    if (usage & static_cast<u32>(wgpu::TextureUsage::CopyDst)) {
        wgpu_usage |= wgpu::TextureUsage::CopyDst;
    }
    if (usage & static_cast<u32>(wgpu::TextureUsage::TextureBinding)) {
        wgpu_usage |= wgpu::TextureUsage::TextureBinding;
    }
    if (usage & static_cast<u32>(wgpu::TextureUsage::StorageBinding)) {
        wgpu_usage |= wgpu::TextureUsage::StorageBinding;
    }
    if (usage & static_cast<u32>(wgpu::TextureUsage::RenderAttachment)) {
        wgpu_usage |= wgpu::TextureUsage::RenderAttachment;
    }
    if (usage & static_cast<u32>(wgpu::TextureUsage::TransientAttachment)) {
        wgpu_usage |= wgpu::TextureUsage::TransientAttachment;
    }
    if (usage & static_cast<u32>(wgpu::TextureUsage::StorageAttachment)) {
        wgpu_usage |= wgpu::TextureUsage::StorageAttachment;
    }
    wgpu_descriptor.usage = wgpu_usage;

    return wgpu_descriptor;
}

struct GPUTexture::Impl {
    wgpu::Texture texture = { nullptr };
    GPUIntegerCoordinateOut width;
    GPUIntegerCoordinateOut height;
    GPUIntegerCoordinateOut depth_or_array_layers;
    GPUIntegerCoordinateOut mip_level_count;
    GPUSize32Out sample_count;
    Bindings::GPUTextureDimension dimension;
    Bindings::GPUTextureFormat format;
    GPUTextureUsageFlags usage;
    String label;
};

GPUTexture::GPUTexture(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUTexture::~GPUTexture() = default;

JS::ThrowCompletionOr<GC::Ref<GPUTexture>> GPUTexture::create(JS::Realm& realm, wgpu::Texture texture)
{
    Bindings::GPUTextureDimension dimension {};
    switch (texture.GetDimension()) {
    case wgpu::TextureDimension::e1D:
        dimension = Bindings::GPUTextureDimension::_1d;
        break;
    case wgpu::TextureDimension::e2D:
        dimension = Bindings::GPUTextureDimension::_2d;
        break;
    case wgpu::TextureDimension::e3D:
        dimension = Bindings::GPUTextureDimension::_3d;
        break;
    default:
        break;
    }

    // FIXME: Support remaining texture formats
    Bindings::GPUTextureFormat format {};
    switch (texture.GetFormat()) {
    case wgpu::TextureFormat::BGRA8Unorm:
        format = Bindings::GPUTextureFormat::Bgra8unorm;
        break;
    default:
        break;
    }

    auto width = texture.GetWidth();
    auto height = texture.GetHeight();
    auto depth_or_array_layers = texture.GetDepthOrArrayLayers();
    auto mip_level_count = texture.GetMipLevelCount();
    auto sample_count = texture.GetSampleCount();
    auto usage = static_cast<GPUTextureUsageFlags>(texture.GetUsage());

    return realm.create<GPUTexture>(realm, Impl { .texture = move(texture), .width = width, .height = height, .depth_or_array_layers = depth_or_array_layers, .mip_level_count = mip_level_count, .sample_count = sample_count, .dimension = dimension, .format = format, .usage = usage, .label = ""_string });
}

void GPUTexture::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUTexture);
    Base::initialize(realm);
}

void GPUTexture::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::Texture GPUTexture::as_wgpu() const
{
    return m_impl->texture;
}

String const& GPUTexture::label() const
{
    return m_impl->label;
}

void GPUTexture::set_label(String const& label)
{
    m_impl->label = label;
}

GPUIntegerCoordinateOut GPUTexture::width() const
{
    return m_impl->width;
}

GPUIntegerCoordinateOut GPUTexture::height() const
{
    return m_impl->height;
}

GPUIntegerCoordinateOut GPUTexture::depth_or_array_layers() const
{
    return m_impl->depth_or_array_layers;
}

GPUIntegerCoordinateOut GPUTexture::mip_level_count() const
{
    return m_impl->mip_level_count;
}

GPUSize32Out GPUTexture::sample_count() const
{
    return m_impl->sample_count;
}

Bindings::GPUTextureDimension GPUTexture::dimension() const
{
    return m_impl->dimension;
}

Bindings::GPUTextureFormat GPUTexture::format() const
{
    return m_impl->format;
}

GPUTextureUsageFlags GPUTexture::usage() const
{
    return m_impl->usage;
}

}
