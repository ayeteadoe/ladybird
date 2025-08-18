/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUTexture.h>
#include <LibWeb/WebGPU/GPUTextureView.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUTexture);

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

wgpu::Texture GPUTexture::wgpu() const
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

// https://www.w3.org/TR/webgpu/#dom-gputexture-createview
// FIXME: Spec comments
GC::Ref<GPUTextureView> GPUTexture::create_view(GPUTextureViewDescriptor const&)
{
    wgpu::TextureView texture_view = nullptr;
    // if (options.has_value()) {
    //     wgpu::TextureViewDescriptor texture_view_descriptor {};
    //
    //     // FIXME: Support remaining texture formats
    //     switch (options->format) {
    //     case Bindings::GPUTextureFormat::Bgra8unorm:
    //         texture_view_descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
    //         break;
    //     default:
    //         break;
    //     }
    //
    //     switch (options->dimension) {
    //     case Bindings::GPUTextureViewDimension::_1d:
    //         texture_view_descriptor.dimension = wgpu::TextureViewDimension::e1D;
    //         break;
    //     case Bindings::GPUTextureViewDimension::_2d:
    //         texture_view_descriptor.dimension = wgpu::TextureViewDimension::e2D;
    //         break;
    //     case Bindings::GPUTextureViewDimension::_2dArray:
    //         texture_view_descriptor.dimension = wgpu::TextureViewDimension::e2DArray;
    //         break;
    //     case Bindings::GPUTextureViewDimension::Cube:
    //         texture_view_descriptor.dimension = wgpu::TextureViewDimension::Cube;
    //         break;
    //     case Bindings::GPUTextureViewDimension::CubeArray:
    //         texture_view_descriptor.dimension = wgpu::TextureViewDimension::CubeArray;
    //         break;
    //     case Bindings::GPUTextureViewDimension::_3d:
    //         texture_view_descriptor.dimension = wgpu::TextureViewDimension::e3D;
    //         break;
    //     default:
    //         break;
    //     }
    //
    //     wgpu::TextureUsage texture_usage = wgpu::TextureUsage::None;
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::CopySrc)) {
    //         texture_usage |= wgpu::TextureUsage::CopySrc;
    //     }
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::CopyDst)) {
    //         texture_usage |= wgpu::TextureUsage::CopyDst;
    //     }
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::TextureBinding)) {
    //         texture_usage |= wgpu::TextureUsage::TextureBinding;
    //     }
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::StorageBinding)) {
    //         texture_usage |= wgpu::TextureUsage::StorageBinding;
    //     }
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::RenderAttachment)) {
    //         texture_usage |= wgpu::TextureUsage::RenderAttachment;
    //     }
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::TransientAttachment)) {
    //         texture_usage |= wgpu::TextureUsage::TransientAttachment;
    //     }
    //     if (options->usage & static_cast<u64>(wgpu::TextureUsage::StorageAttachment)) {
    //         texture_usage |= wgpu::TextureUsage::StorageAttachment;
    //     }
    //     texture_view_descriptor.usage = texture_usage;
    //
    //     switch (options->aspect) {
    //     case Bindings::GPUTextureAspect::All:
    //         texture_view_descriptor.aspect = wgpu::TextureAspect::All;
    //         break;
    //     case Bindings::GPUTextureAspect::DepthOnly:
    //         texture_view_descriptor.aspect = wgpu::TextureAspect::DepthOnly;
    //         break;
    //     case Bindings::GPUTextureAspect::StencilOnly:
    //         texture_view_descriptor.aspect = wgpu::TextureAspect::StencilOnly;
    //         break;
    //     default:
    //         break;
    //     }
    //     texture_view_descriptor.baseMipLevel = options->base_mip_level;
    //     texture_view_descriptor.mipLevelCount = options->mip_level_count.value_or(wgpu::kMipLevelCountUndefined);
    //     texture_view_descriptor.baseArrayLayer = options->base_array_layer;
    //     texture_view_descriptor.arrayLayerCount = options->array_layer_count.value_or(wgpu::kArrayLayerCountUndefined);
    //     texture_view = m_impl->texture.CreateView(&texture_view_descriptor);
    // }
    // else {
    //     texture_view = m_impl->texture.CreateView();
    // }
    // FIXME: Make an empty/unspecified options equivalent to calling CreateView() with nullptr
    texture_view = m_impl->texture.CreateView(nullptr);
    auto& realm = this->realm();
    return MUST(GPUTextureView::create(realm, move(texture_view)));
}

}
