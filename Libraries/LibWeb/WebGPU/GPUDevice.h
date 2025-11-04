/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>
#include <LibWeb/WebGPU/Native/NativeGPUDevice.h>

namespace Web::WebGPU {

class GPUDevice final : public DOM::EventTarget {
    WEB_PLATFORM_OBJECT(GPUDevice, DOM::EventTarget);
    GC_DECLARE_ALLOCATOR(GPUDevice);

    static JS::ThrowCompletionOr<GC::Ref<GPUDevice>> create(JS::Realm&, NativeGPUDevice);

    GC::Ref<WebIDL::Promise> lost() const;

    String const& label() const;
    void set_label(String const& label);

    NativeGPUDevice& native_gpu_device() { return m_native_gpu_device; }

    GC::Ref<GPUCommandEncoder> create_command_encoder(Optional<GPUCommandEncoderDescriptor> descriptor = {}) const;

private:
    explicit GPUDevice(JS::Realm&, NativeGPUDevice);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUDevice m_native_gpu_device;
};

}
