/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebIDL/Promise.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUDevice);

GPUDevice::GPUDevice(JS::Realm& realm, NativeGPUDevice native_gpu_device)
    : EventTarget(realm)
    , m_native_gpu_device(move(native_gpu_device))
{
}

JS::ThrowCompletionOr<GC::Ref<GPUDevice>> GPUDevice::create(JS::Realm& realm, NativeGPUDevice native_gpu_device)
{
    return realm.create<GPUDevice>(realm, move(native_gpu_device));
}

void GPUDevice::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUDevice);
    EventTarget::initialize(realm);
}

void GPUDevice::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-lost
GC::Ref<WebIDL::Promise> GPUDevice::lost() const
{
    // FIXME: Implement specification
    GC::Ref promise = WebIDL::create_promise(realm());
    return promise;
}

String const& GPUDevice::label() const
{
    return m_native_gpu_device.label();
}

void GPUDevice::set_label(String const& label)
{
    m_native_gpu_device.set_label(label);
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createcommandencoder
GC::Ref<GPUCommandEncoder> GPUDevice::create_command_encoder(Optional<GPUCommandEncoderDescriptor> descriptor) const
{
    return MUST(GPUCommandEncoder::create(realm(), m_native_gpu_device.create_command_encoder(descriptor)));
}

}
