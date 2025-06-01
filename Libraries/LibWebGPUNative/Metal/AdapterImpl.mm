/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Metal/AdapterImpl.h>
#include <LibWebGPUNative/Metal/Error.h>
#import <Metal/Metal.h>

namespace WebGPUNative {

MetalDeviceHandle::MetalDeviceHandle(id device) : m_device(device) {
    [m_device retain];
}

MetalDeviceHandle::~MetalDeviceHandle() {
    if (m_device) {
        [m_device release];
        m_device = nullptr;
    }
}

MetalDeviceHandle::MetalDeviceHandle(MetalDeviceHandle&& other) noexcept : m_device(other.m_device) {
    other.m_device = nullptr;
}

MetalDeviceHandle& MetalDeviceHandle::operator=(MetalDeviceHandle&& other) noexcept {
    if (this != &other) {
        if (m_device) {
            [m_device release];
        }
        m_device = other.m_device;
        other.m_device = nullptr;
    }
    return *this;
}

Adapter::Impl::Impl(Instance const&) {
}

ErrorOr<void> Adapter::Impl::initialize() {
    NSArray<id<MTLDevice>> *devices = MTLCopyAllDevices();
    if (!devices || devices.count == 0) {
        [devices release];
        return make_error("No Metal devices found");
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
        return make_error("No supported Metal devices available");
    }

    m_metal_device = make<MetalDeviceHandle>(selected_device);

    // TODO: Track device consumption state

    return {};
}

}
