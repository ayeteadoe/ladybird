/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/NativeGPUQueue.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPUQueue::Impl {
    wgpu::Queue m_queue { nullptr };

    String m_label;

    String const& label() const;
    void set_label(String const& label);

    void submit(GC::RootVector<GC::Root<GPUCommandBuffer>> const& command_buffers);

    void write_buffer(GC::Root<GPUBuffer> buffer, WebIDL::UnsignedLongLong buffer_offset, GC::Root<WebIDL::BufferSource> const& data, Optional<WebIDL::UnsignedLongLong> data_offset, Optional<WebIDL::UnsignedLongLong> size);
};

}
