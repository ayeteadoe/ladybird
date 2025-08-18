/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUTextureView.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUTextureView);

wgpu::TextureViewDescriptor GPUTextureViewDescriptor::to_wgpu() const
{
    wgpu::TextureViewDescriptor wgpu_descriptor {};

    // FIXME: Support remaining texture formats
    switch (format) {
    case Bindings::GPUTextureFormat::Bgra8unorm:
        wgpu_descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
        break;
    default:
        break;
    }

    switch (dimension) {
    case Bindings::GPUTextureViewDimension::_1d:
        wgpu_descriptor.dimension = wgpu::TextureViewDimension::e1D;
        break;
    case Bindings::GPUTextureViewDimension::_2d:
        wgpu_descriptor.dimension = wgpu::TextureViewDimension::e2D;
        break;
    case Bindings::GPUTextureViewDimension::_2dArray:
        wgpu_descriptor.dimension = wgpu::TextureViewDimension::e2DArray;
        break;
    case Bindings::GPUTextureViewDimension::Cube:
        wgpu_descriptor.dimension = wgpu::TextureViewDimension::Cube;
        break;
    case Bindings::GPUTextureViewDimension::CubeArray:
        wgpu_descriptor.dimension = wgpu::TextureViewDimension::CubeArray;
        break;
    case Bindings::GPUTextureViewDimension::_3d:
        wgpu_descriptor.dimension = wgpu::TextureViewDimension::e3D;
        break;
    default:
        break;
    }

    wgpu::TextureUsage wgpu_usage = wgpu::TextureUsage::None;
    if (usage & static_cast<u64>(wgpu::TextureUsage::CopySrc)) {
        wgpu_usage |= wgpu::TextureUsage::CopySrc;
    }
    if (usage & static_cast<u64>(wgpu::TextureUsage::CopyDst)) {
        wgpu_usage |= wgpu::TextureUsage::CopyDst;
    }
    if (usage & static_cast<u64>(wgpu::TextureUsage::TextureBinding)) {
        wgpu_usage |= wgpu::TextureUsage::TextureBinding;
    }
    if (usage & static_cast<u64>(wgpu::TextureUsage::StorageBinding)) {
        wgpu_usage |= wgpu::TextureUsage::StorageBinding;
    }
    if (usage & static_cast<u64>(wgpu::TextureUsage::RenderAttachment)) {
        wgpu_usage |= wgpu::TextureUsage::RenderAttachment;
    }
    if (usage & static_cast<u64>(wgpu::TextureUsage::TransientAttachment)) {
        wgpu_usage |= wgpu::TextureUsage::TransientAttachment;
    }
    if (usage & static_cast<u64>(wgpu::TextureUsage::StorageAttachment)) {
        wgpu_usage |= wgpu::TextureUsage::StorageAttachment;
    }
    wgpu_descriptor.usage = wgpu_usage;

    switch (aspect) {
    case Bindings::GPUTextureAspect::All:
        wgpu_descriptor.aspect = wgpu::TextureAspect::All;
        break;
    case Bindings::GPUTextureAspect::DepthOnly:
        wgpu_descriptor.aspect = wgpu::TextureAspect::DepthOnly;
        break;
    case Bindings::GPUTextureAspect::StencilOnly:
        wgpu_descriptor.aspect = wgpu::TextureAspect::StencilOnly;
        break;
    default:
        break;
    }
    wgpu_descriptor.baseMipLevel = base_mip_level;
    wgpu_descriptor.mipLevelCount = mip_level_count.value_or(wgpu::kMipLevelCountUndefined);
    wgpu_descriptor.baseArrayLayer = base_array_layer;
    wgpu_descriptor.arrayLayerCount = array_layer_count.value_or(wgpu::kArrayLayerCountUndefined);

    return wgpu_descriptor;
}

struct GPUTextureView::Impl {
    wgpu::TextureView texture_view = { nullptr };
    String label;
};

GPUTextureView::GPUTextureView(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUTextureView::~GPUTextureView() = default;

JS::ThrowCompletionOr<GC::Ref<GPUTextureView>> GPUTextureView::create(JS::Realm& realm, wgpu::TextureView texture_view)
{
    return realm.create<GPUTextureView>(realm, Impl { .texture_view = move(texture_view), .label = ""_string });
}

void GPUTextureView::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUTextureView);
    Base::initialize(realm);
}

void GPUTextureView::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::TextureView GPUTextureView::as_wgpu() const
{
    return m_impl->texture_view;
}

String const& GPUTextureView::label() const
{
    return m_impl->label;
}

void GPUTextureView::set_label(String const& label)
{
    m_impl->label = label;
}

}
