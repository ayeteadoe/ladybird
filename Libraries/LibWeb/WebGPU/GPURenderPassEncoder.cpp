/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GPUTextureView.h"

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPURenderPassEncoder.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPURenderPassEncoder);

struct GPURenderPassEncoder::Impl {
    wgpu::RenderPassEncoder render_pass_encoder = { nullptr };
    String label;
};

GPURenderPassEncoder::GPURenderPassEncoder(JS::Realm& realm, Impl impl)
    : PlatformObject(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPURenderPassEncoder::~GPURenderPassEncoder() = default;

JS::ThrowCompletionOr<GC::Ref<GPURenderPassEncoder>> GPURenderPassEncoder::create(JS::Realm& realm, wgpu::RenderPassEncoder render_pass_encoder)
{
    return realm.create<GPURenderPassEncoder>(realm, Impl { .render_pass_encoder = move(render_pass_encoder), .label = ""_string });
}

void GPURenderPassEncoder::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPURenderPassEncoder);
    Base::initialize(realm);
}

void GPURenderPassEncoder::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

wgpu::RenderPassEncoder GPURenderPassEncoder::as_wgpu() const
{
    return m_impl->render_pass_encoder;
}

String const& GPURenderPassEncoder::label() const
{
    return m_impl->label;
}

void GPURenderPassEncoder::set_label(String const& label)
{
    m_impl->label = label;
}

// https://www.w3.org/TR/webgpu/#dom-gpurenderpassencoder-end
// FIXME: Spec comments
void GPURenderPassEncoder::end()
{
    m_impl->render_pass_encoder.End();
}

}
