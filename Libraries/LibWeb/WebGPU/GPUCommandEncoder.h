/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUCommandEncoderPrototype.h>
#include <LibWeb/Bindings/GPUTextureViewPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/GPUObjectBase.h>
#include <LibWeb/WebIDL/Types.h>

namespace wgpu {

class CommandEncoder;
struct CommandEncoderDescriptor;
struct TexelCopyBufferLayout;
struct TexelCopyBufferInfo;
struct TexelCopyTextureInfo;

}

namespace Web::WebGPU {

struct GPUTexelCopyBufferLayout {
    WebIDL::UnsignedLongLong offset = 0;
    Optional<WebIDL::UnsignedLong> bytes_per_row;
    Optional<WebIDL::UnsignedLong> rows_per_image;
    wgpu::TexelCopyBufferLayout to_wgpu() const;
};

struct GPUTexelCopyBufferInfo : GPUTexelCopyBufferLayout {
    GC::Ptr<GPUBuffer> buffer;
    wgpu::TexelCopyBufferInfo to_wgpu() const;
};

struct GPUTexelCopyTextureInfo {
    GC::Ptr<GPUTexture> texture;
    GPUIntegerCoordinate mip_level = 0;
    GPUOrigin3D origin {};
    Bindings::GPUTextureAspect aspect = Bindings::GPUTextureAspect::All;
    wgpu::TexelCopyTextureInfo to_wgpu() const;
};

struct GPUCommandEncoderDescriptor : GPUObjectDescriptorBase {
    wgpu::CommandEncoderDescriptor to_wgpu() const;
};

class GPUCommandEncoder final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCommandEncoder, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCommandEncoder);

    static JS::ThrowCompletionOr<GC::Ref<GPUCommandEncoder>> create(JS::Realm&, wgpu::CommandEncoder);

    ~GPUCommandEncoder() override;

    wgpu::CommandEncoder as_wgpu() const;

    String const& label() const;
    void set_label(String const& label);

    GC::Ref<GPURenderPassEncoder> begin_render_pass(GPURenderPassDescriptor const& descriptor) const;

    void copy_texture_to_buffer(GPUTexelCopyTextureInfo const&, GPUTexelCopyBufferInfo const&, GPUExtent3D const&);

    GC::Ref<GPUCommandBuffer> finish(GPUCommandBufferDescriptor const& descriptor);

private:
    struct Impl;
    explicit GPUCommandEncoder(JS::Realm&, Impl);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NonnullOwnPtr<Impl> m_impl;
};

};
