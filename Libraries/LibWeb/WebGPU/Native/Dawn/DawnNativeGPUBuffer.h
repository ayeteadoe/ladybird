/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUBuffer.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUBuffer::Impl {
    wgpu::Buffer m_buffer { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);

    WebIDL::UnsignedLongLong size() const;
    WebIDL::UnsignedLong usage() const;
    Bindings::GPUBufferMapState map_state() const;

    void unmap();
};

}
