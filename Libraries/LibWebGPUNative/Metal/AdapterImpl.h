/*
* Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWebGPUNative/Forward.h>

#ifdef __OBJC__
@protocol MTLDevice;
#endif

namespace WebGPUNative {

class AdapterImplBridge {
public:
    AdapterImplBridge();
    ~AdapterImplBridge();

    bool initialize();
    void* get_metal_device();

private:
    void* m_swift_impl; // Opaque pointer to Swift AdapterImpl
};

#ifdef __OBJC__
id<MTLDevice> metal_device_from_opaque(void* opaque_device);
#endif

}