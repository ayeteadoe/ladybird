/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/OwnPtr.h>
#include <LibWebGPUNative/Adapter.h>
#include <LibWebGPUNative/Metal/Handle.h>

namespace WebGPUNative {

struct Adapter::Impl {
    explicit Impl(Instance const& gpu);
    ~Impl() = default;

    ErrorOr<void> initialize();

    id metal_device() const { return m_metal_device->get(); }

private:
    OwnPtr<MetalDeviceHandle> m_metal_device;
};

}
