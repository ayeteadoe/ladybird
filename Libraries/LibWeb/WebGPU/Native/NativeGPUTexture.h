/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibWeb/Bindings/GPUTexturePrototype.h>
#include <LibWeb/WebGPU/Native/NativeGPUTextureView.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebGPU {

struct GPUExtent3DDict {
    WebIDL::UnsignedLong width;
    WebIDL::UnsignedLong height = 1;
    WebIDL::UnsignedLong depth_or_array_layers = 1;
};

using GPUExtent3D = Variant<Vector<WebIDL::UnsignedLong>, GPUExtent3DDict, Empty>;

struct GPUTextureDescriptor : GPUObjectDescriptorBase {
    GPUExtent3D size;
    WebIDL::UnsignedLong mip_level_count = 1;
    WebIDL::UnsignedLong sample_count = 1;
    Bindings::GPUTextureDimension dimension = Bindings::GPUTextureDimension::_2d;
    Bindings::GPUTextureFormat format;
    WebIDL::UnsignedLong usage;
    Vector<Bindings::GPUTextureFormat> view_formats;
};

class NativeDrawingBuffer;
class NativeGPUDevice;

class NativeGPUTexture {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeGPUTexture);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeGPUTexture);
    WEBGPU_NATIVE_DECLARE_GPUOBJECT(NativeGPUTexture);

public:
    friend NativeGPUDevice;

    static NativeGPUTexture create();

    // We distinguish between creating a texture for a shared drawing buffer vs a texture from a standard texture descriptor to ensure the texture view descriptor that will be passed to rendering pass is compatible with the drawing buffer texture
    static NativeGPUTexture create_from_drawing_buffer(NativeDrawingBuffer& drawing_buffer);

    bool is_drawing_buffer() const;

    NativeGPUTextureView create_view(Optional<GPUTextureViewDescriptor> descriptor);
};

}
