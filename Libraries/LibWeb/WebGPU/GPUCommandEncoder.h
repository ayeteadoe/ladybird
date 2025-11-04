/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUCommandEncoderPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/WebGPU/Native/NativeGPUCommandEncoder.h>

namespace Web::WebGPU {

class GPUCommandEncoder final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUCommandEncoder, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUCommandEncoder);

    static JS::ThrowCompletionOr<GC::Ref<GPUCommandEncoder>> create(JS::Realm&, NativeGPUCommandEncoder);

    String const& label() const;
    void set_label(String const& label);

    NativeGPUCommandEncoder& native_gpu_command_encoder() { return m_native_gpu_command_encoder; }

private:
    explicit GPUCommandEncoder(JS::Realm&, NativeGPUCommandEncoder);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUCommandEncoder m_native_gpu_command_encoder;
};

}
