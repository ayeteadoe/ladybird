/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUTexturePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>

namespace wgpu {

class Texture;
struct TextureDescriptor;

}

namespace Web::WebGPU {

using GPUTextureUsageFlags = u32;

struct GPUTextureDescriptor : GPUObjectDescriptorBase {
    GPUExtent3D size;

    GPUIntegerCoordinate mip_level_count = 1;
    GPUSize32 sample_count = 1;
    Bindings::GPUTextureDimension dimension = Bindings::GPUTextureDimension::_2d;
    Bindings::GPUTextureFormat format;
    GPUTextureUsageFlags usage;
    Vector<Bindings::GPUTextureFormat> view_formats;
};

class GPUTexture final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUTexture, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUTexture);

    static JS::ThrowCompletionOr<GC::Ref<GPUTexture>> create(JS::Realm&, wgpu::Texture);

    ~GPUTexture() override;

    wgpu::Texture wgpu() const;

    String const& label() const;
    void set_label(String const& label);

    GPUIntegerCoordinateOut width() const;
    GPUIntegerCoordinateOut height() const;
    GPUIntegerCoordinateOut depth_or_array_layers() const;
    GPUIntegerCoordinateOut mip_level_count() const;
    GPUSize32Out sample_count() const;
    Bindings::GPUTextureDimension dimension() const;
    Bindings::GPUTextureFormat format() const;
    GPUTextureUsageFlags usage() const;

private:
    struct Impl;
    explicit GPUTexture(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
