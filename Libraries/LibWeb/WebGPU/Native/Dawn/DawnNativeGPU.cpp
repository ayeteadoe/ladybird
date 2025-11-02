/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Vector.h>
#include <LibJS/Runtime/Realm.h>
#include <LibWeb/HTML/Scripting/TemporaryExecutionContext.h>
#include <LibWeb/Platform/EventLoopPlugin.h>
#include <LibWeb/WebGPU/GPUAdapter.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUAdapter.h>
#include <LibWeb/WebGPU/Native/NativeGPU.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

struct NativeGPU::Impl {
    wgpu::Instance m_instance { nullptr };
};

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPU);

NativeGPU NativeGPU::create()
{
    Vector const required_features = {
        // See Callback Reentrancy section at https://webgpu-native.github.io/webgpu-headers/Asynchronous-Operations.html#Process-Events
        // See WGPUCallbackMode enum descriptions at https://webgpu-native.github.io/webgpu-headers/group__Enumerations.html
        // This ensures we get the implicit safety and avoid undefined behaviour, so we will just use WaitAnyOnly inside an EventLoopPlugin::deferred_invoke()
        // for executing asynchronous operations on the instance with an infinite timeout.
        wgpu::InstanceFeatureName::TimedWaitAny,
    };

    wgpu::InstanceDescriptor instance_descriptor {};
    instance_descriptor.requiredFeatureCount = required_features.size();
    instance_descriptor.requiredFeatures = required_features.data();

    return NativeGPU { Impl { .m_instance = wgpu::CreateInstance(&instance_descriptor) } };
}

// https://gpuweb.github.io/gpuweb/#dom-gpu-requestadapter
void NativeGPU::request_adapter_initialization_steps(JS::Realm& realm, GC::Ref<WebIDL::Promise>& promise, Optional<GPURequestAdapterOptions> options)
{
    Platform::EventLoopPlugin::the().deferred_invoke(GC::create_function(realm.heap(), [this, options, &realm, promise]() mutable {
        wgpu::RequestAdapterOptions request_adapter_options = {};

        if (options.has_value()) {
            // https://gpuweb.github.io/gpuweb/#dom-gpurequestadapteroptions-featurelevel
            if (options->feature_level == "core"sv) {
                request_adapter_options.featureLevel = wgpu::FeatureLevel::Core;
            } else if (options->feature_level == "compatibility"sv) {
                request_adapter_options.featureLevel = wgpu::FeatureLevel::Compatibility;
            }

            switch (options->power_preference) {
            case Bindings::GPUPowerPreference::HighPerformance: {
                request_adapter_options.powerPreference = wgpu::PowerPreference::HighPerformance;
                break;
            }
            case Bindings::GPUPowerPreference::LowPower: {
                request_adapter_options.powerPreference = wgpu::PowerPreference::LowPower;
                break;
            }
            default:
                break;
            }

            request_adapter_options.forceFallbackAdapter = options->force_fallback_adapter;
        }
        m_impl->m_instance.WaitAny(m_impl->m_instance.RequestAdapter(options.has_value() ? &request_adapter_options : nullptr, wgpu::CallbackMode::WaitAnyOnly, [request_adapter_options, realm = GC::Root(realm), promise = GC::Root(promise)](wgpu::RequestAdapterStatus status, wgpu::Adapter native_adapter, char const* message) {
            GC::Ptr<GPUAdapter> adapter;

            // Device timeline initialization steps:
            //  1. All of the requirements in the following steps must be met.
            //      1. options.featureLevel must be a feature level string.
            if (status == wgpu::RequestAdapterStatus::Success) {
                // If they are met and the user agent chooses to return an adapter:
                //     1. Set adapter to an adapter chosen according to the rules in Adapter Selection (https://gpuweb.github.io/gpuweb/#adapter-selection) and the criteria in options, adhering to Adapter Capability Guarantees (https://gpuweb.github.io/gpuweb/#adapter-capability-guarantees). Initialize the properties of adapter according to their definitions:
                //          1. Set adapter.[[limits]] and adapter.[[features]] according to the supported capabilities of the adapter. adapter.[[features]] must contain "core-features-and-limits".
                //          2. If adapter meets the criteria of a fallback adapter set adapter.[[fallback]] to true. Otherwise, set it to false.
                //          3. FIXME: Set adapter.[[xrCompatible]] to options.xrCompatible.
                auto native_gpu_adapter = NativeGPUAdapter::create();
                native_gpu_adapter.m_impl->m_adapter = native_adapter;
                adapter = MUST(GPUAdapter::create(*realm, move(native_gpu_adapter)));
            } else {
                dbgln("Unable to request adapter: {}", message);
                // Otherwise:
                //     1. Let adapter be null.
                adapter = nullptr;
            }

            // 2. Issue the subsequent steps on contentTimeline.

            // Content timeline steps:
            //     1. If adapter is not null:
            if (adapter != nullptr) {
                //     1. Resolve promise with a new GPUAdapter encapsulating adapter.
                auto& gpu_adapter_realm = HTML::relevant_realm(*adapter);
                HTML::TemporaryExecutionContext const context { gpu_adapter_realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::resolve_promise(gpu_adapter_realm, *promise, adapter);
            } else {
                // 2. Otherwise, Resolve promise with null.
                HTML::TemporaryExecutionContext const context { *realm, HTML::TemporaryExecutionContext::CallbacksEnabled::Yes };
                WebIDL::resolve_promise(*realm, *promise, JS::js_null());
            }
        }),
            UINT64_MAX);
    }));
}

}
