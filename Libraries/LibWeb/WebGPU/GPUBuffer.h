/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUBufferPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUBuffer.h>

namespace Web::WebGPU {

class GPUBuffer final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUBuffer, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUBuffer);

    static JS::ThrowCompletionOr<GC::Ref<GPUBuffer>> create(JS::Realm&, NativeGPUBuffer);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUBuffer& native_gpu_buffer() { return m_native_gpu_buffer; }

    WebIDL::UnsignedLongLong size() const;

    WebIDL::UnsignedLong usage() const;

    Bindings::GPUBufferMapState map_state() const;

private:
    explicit GPUBuffer(JS::Realm&, NativeGPUBuffer);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUBuffer m_native_gpu_buffer;
};

}
