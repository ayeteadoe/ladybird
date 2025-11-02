/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/Native/NativeGPUAdapter.h>
#include <LibWeb/WebGPU/Native/NativeGPUDevice.h>

namespace Web::WebGPU {

class GPUAdapter final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUAdapter, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUAdapter);

    static JS::ThrowCompletionOr<GC::Ref<GPUAdapter>> create(JS::Realm&, NativeGPUAdapter);

    NativeGPUAdapter& native_gpu_adapter() { return m_native_gpu_adapter; }

    GC::Ref<WebIDL::Promise> request_device(Optional<GPUDeviceDescriptor> descriptor);

private:
    explicit GPUAdapter(JS::Realm&, NativeGPUAdapter);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    NativeGPUAdapter m_native_gpu_adapter;
};

}
