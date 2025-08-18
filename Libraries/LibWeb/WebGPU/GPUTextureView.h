/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUPrototype.h>
#include <LibWeb/Bindings/GPUTextureViewPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class TextureView;
struct TextureViewDescriptor;

}

namespace Web::WebGPU {

using GPUTextureUsageFlags = u32;

struct GPUTextureViewDescriptor : GPUObjectDescriptorBase {
    Bindings::GPUTextureFormat format;
    Bindings::GPUTextureViewDimension dimension;
    GPUTextureUsageFlags usage;
    Bindings::GPUTextureAspect aspect = Bindings::GPUTextureAspect::All;
    GPUIntegerCoordinate base_mip_level = 0;
    Optional<GPUIntegerCoordinate> mip_level_count;
    GPUIntegerCoordinate base_array_layer = 0;
    Optional<GPUIntegerCoordinate> array_layer_count;
};

class GPUTextureView final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUTextureView, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUTextureView);

    static JS::ThrowCompletionOr<GC::Ref<GPUTextureView>> create(JS::Realm&, wgpu::TextureView);

    ~GPUTextureView() override;

    wgpu::TextureView wgpu() const;

    String const& label() const;
    void set_label(String const& label);

private:
    struct Impl;
    explicit GPUTextureView(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
