/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/HTML/Scripting/TemporaryExecutionContext.h>
#include <LibWeb/Platform/EventLoopPlugin.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUAdapter.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUDevice.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUAdapter);

// https://www.w3.org/TR/webgpu/#dom-gpuadapter-requestdevice
void NativeGPUAdapter::Impl::request_device_initialization_steps(JS::Realm& realm, GC::Ref<WebIDL::Promise>& promise, Optional<GPUDeviceDescriptor> descriptor)
{
    Platform::EventLoopPlugin::the().deferred_invoke(GC::create_function(realm.heap(), [this, descriptor, &realm, promise]() mutable {
        wgpu::DeviceDescriptor device_descriptor;
        String device_label;
        if (descriptor.has_value()) {
            device_label = descriptor.value().label;
            auto const device_label_view = device_label.bytes_as_string_view();
            device_descriptor = { wgpu::DeviceDescriptor::Init { .nextInChain = nullptr, .label = wgpu::StringView { device_label_view.characters_without_null_termination(), device_label_view.length() }, .defaultQueue = wgpu::QueueDescriptor {} } };
        }

        // FIXME: https://www.w3.org/TR/webgpu/#dom-gpudevice-lost
        device_descriptor.SetDeviceLostCallback(
            wgpu::CallbackMode::AllowSpontaneous,
            [](wgpu::Device const&, wgpu::DeviceLostReason reason, wgpu::StringView message) {
                auto log_device_lost = [message](StringView reason) {
                    dbgln("Device lost because of {}: {}", reason, StringView { message.data, message.length });
                };

                switch (reason) {
                case wgpu::DeviceLostReason::Unknown:
                    log_device_lost("Unknown"sv);
                    break;
                case wgpu::DeviceLostReason::Destroyed:
                    log_device_lost("Destroyed"sv);
                    break;
                case wgpu::DeviceLostReason::CallbackCancelled:
                    log_device_lost("CallbackCancelled"sv);
                    break;
                case wgpu::DeviceLostReason::FailedCreation:
                    log_device_lost("FailedCreation"sv);
                    break;
                default:
                    VERIFY_NOT_REACHED();
                }
            });
        // FIXME: https://www.w3.org/TR/webgpu/#eventdef-gpudevice-uncapturederror
        device_descriptor.SetUncapturedErrorCallback(
            [](wgpu::Device const&, wgpu::ErrorType type, wgpu::StringView message) {
                auto log_uncaptured_error = [message](StringView error_type) {
                    dbgln("{} error: {}", error_type, StringView { message.data, message.length });
                };

                switch (type) {
                case wgpu::ErrorType::Validation:
                    log_uncaptured_error("Validation"sv);
                    break;
                case wgpu::ErrorType::OutOfMemory:
                    log_uncaptured_error("Out of memory"sv);
                    break;
                case wgpu::ErrorType::Internal:
                    log_uncaptured_error("Internal"sv);
                    break;
                case wgpu::ErrorType::Unknown:
                    log_uncaptured_error("Unknown"sv);
                    break;
                default:
                    VERIFY_NOT_REACHED();
                }
            });
        m_adapter.GetInstance().WaitAny(m_adapter.RequestDevice(&device_descriptor, wgpu::CallbackMode::WaitAnyOnly, [this, device_label, realm = GC::Root(realm), promise = GC::Root(promise)](wgpu::RequestDeviceStatus status, wgpu::Device native_device, char const* message) {
            // Device timeline initialization steps:
            // 1. FIXME: If any of the following requirements are unmet:
            //     - The set of values in descriptor.requiredFeatures must be a subset of those in adapter.[[features]].

            // 2. All of the requirements in the following steps must be met.
            //     1. adapter.[[state]] must not be "consumed".
            if (m_state == State::Consumed) {
                HTML::TemporaryExecutionContext const context { *realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::reject_promise(*realm, *promise, WebIDL::OperationError::create(*realm, "Adapter has already been consumed"_utf16));
                return;
            }
            //     2. FIXME: For each [key, value] in descriptor.requiredLimits for which value is not undefined:
            //         1. key must be the name of a member of supported limits.
            //         2. value must be no better than adapter.[[limits]][key].
            //         If key’s class is alignment, value must be a power of 2 less than 2^32.

            // 3. If adapter.[[state]] is "expired" or the user agent otherwise cannot fulfill the request:
            if (m_state == State::Expired || status != wgpu::RequestDeviceStatus::Success) {
                dbgln("Unable to request device: {}", message);

                // 1. Let device be a new device.
                // NOTE: This is the native_device parameter

                // 2. FIXME: Lose the device(device, "unknown").

                // 3. Assert adapter.[[state]] is "expired".
                VERIFY(m_state == State::Expired);
            } else {
                // Otherwise:
                //  1. Let device be a new device with the capabilities described by descriptor.
                // NOTE: This is the native_device parameter

                // 2. Expire adapter.
                m_state = State::Expired;
            }

            // 4. Issue the subsequent steps on contentTimeline.
            //      Content timeline steps:
            //      1. Let gpuDevice be a new GPUDevice instance.
            //      2. Set gpuDevice.[[device]] to device.
            //      3. FIXME: Set device.[[content device]] to gpuDevice.
            //      4. Set gpuDevice.label to descriptor.label.
            auto native_gpu_device = NativeGPUDevice::create();
            native_gpu_device.m_impl->m_device = move(native_device);
            native_gpu_device.set_label(device_label);
            GC::Ref<GPUDevice> gpu_device = MUST(GPUDevice::create(*realm, move(native_gpu_device)));
            //      5. Resolve promise with gpuDevice.
            auto& gpu_device_realm = HTML::relevant_realm(*gpu_device);
            HTML::TemporaryExecutionContext const context { gpu_device_realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
            WebIDL::resolve_promise(gpu_device_realm, *promise, gpu_device);
        }),
            UINT64_MAX);
    }));
}

NativeGPUAdapter NativeGPUAdapter::create()
{
    return NativeGPUAdapter(Impl {});
}

void NativeGPUAdapter::request_device_initialization_steps(JS::Realm& realm, GC::Ref<WebIDL::Promise>& promise, Optional<GPUDeviceDescriptor> descriptor)
{
    m_impl->request_device_initialization_steps(realm, promise, descriptor);
}

}
