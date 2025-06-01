/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Metal/AdapterImpl.h>
#include <LibWebGPUNative/Metal/Error.h>
#import <Metal/Metal.h>

namespace WebGPUNative {

Adapter::Impl::Impl(Instance const&) {
}

ErrorOr<void> Adapter::Impl::initialize() {
    NSArray<id<MTLDevice>> *devices = MTLCopyAllDevices();
    if (!devices || devices.count == 0) {
        [devices release];
        return make_error("No devices found");
    }

    id<MTLDevice> selected_device = nil;

    // TODO: Implement proper adapter selection based on WebGPU options
    // https://www.w3.org/TR/webgpu/#adapter-selection
    for (id<MTLDevice> device in devices) {
        if (!device.isLowPower) {
            selected_device = device;
            break;
        }
    }

    if (!selected_device) {
        for (id<MTLDevice> device in devices) {
            if (device.isLowPower) {
                selected_device = device;
                break;
            }
        }
    }

    if (!selected_device && devices.count > 0) {
        selected_device = devices[0];
    }

    [devices release];

    if (!selected_device) {
        return make_error("No supported devices available");
    }

    m_metal_device = make<MetalDeviceHandle>(selected_device);

    // TODO: Track device consumption state

    return {};
}

}
