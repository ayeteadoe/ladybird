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

wgpu::TextureView GPUTextureView::wgpu() const
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
