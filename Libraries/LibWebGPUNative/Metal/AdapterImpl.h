/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/OwnPtr.h>
#include <LibWebGPUNative/Adapter.h>
#include <objc/objc.h>

namespace WebGPUNative {

class MetalDeviceHandle {
public:
    explicit MetalDeviceHandle(id device);
    ~MetalDeviceHandle();

    MetalDeviceHandle(MetalDeviceHandle const&) = delete;
    MetalDeviceHandle& operator=(MetalDeviceHandle const&) = delete;
    MetalDeviceHandle(MetalDeviceHandle&&) noexcept;
    MetalDeviceHandle& operator=(MetalDeviceHandle&&) noexcept;

    id get() const { return m_device; }

private:
    id m_device;
};

struct Adapter::Impl {
    explicit Impl(Instance const& gpu);
    ~Impl() = default;

    ErrorOr<void> initialize();

    id metal_device() const { return m_metal_device->get(); }

private:
    OwnPtr<MetalDeviceHandle> m_metal_device;
};

}
