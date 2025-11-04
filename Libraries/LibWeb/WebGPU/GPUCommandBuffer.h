/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUCommandBufferPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUCommandBuffer.h>

namespace Web::WebGPU {

class GPUCommandBuffer final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCommandBuffer, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCommandBuffer);

    static JS::ThrowCompletionOr<GC::Ref<GPUCommandBuffer>> create(JS::Realm&, NativeGPUCommandBuffer);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUCommandBuffer& native_gpu_command_buffer() { return m_native_gpu_command_buffer; }

private:
    explicit GPUCommandBuffer(JS::Realm&, NativeGPUCommandBuffer);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUCommandBuffer m_native_gpu_command_buffer;
};

}
