/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Metal/AdapterImpl.h>
#include <LibWebGPUNative/include/LibWebGPUNative-Swift.h>

#import <Metal/Metal.h>

namespace WebGPUNative {

AdapterImplBridge::AdapterImplBridge()
{
    // Create Swift AdapterImpl instance
    auto swift_adapter = WebGPUNative::AdapterImpl::init();
    m_swift_impl = new WebGPUNative::AdapterImpl(std::move(swift_adapter));
}

AdapterImplBridge::~AdapterImplBridge()
{
    if (m_swift_impl) {
        delete static_cast<WebGPUNative::AdapterImpl*>(m_swift_impl);
    }
}

bool AdapterImplBridge::initialize()
{
    auto* swift_adapter = static_cast<WebGPUNative::AdapterImpl*>(m_swift_impl);
    return swift_adapter->initialize();
}

void* AdapterImplBridge::get_metal_device()
{
    auto* swift_adapter = static_cast<WebGPUNative::AdapterImpl*>(m_swift_impl);
    return swift_adapter->getMetalDevice();
}

id<MTLDevice> metal_device_from_opaque(void* opaque_device)
{
    if (!opaque_device) return nil;
    return (__bridge id<MTLDevice>)opaque_device;
}

}
