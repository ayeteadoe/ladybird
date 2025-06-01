/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Adapter.h>
#include <LibWebGPUNative/Metal/AdapterImpl.h>
#include <LibWebGPUNative/Metal/Error.h>

namespace WebGPUNative {

struct Adapter::Impl {
    explicit Impl(Instance const&)
    {
    }

    ErrorOr<void> initialize()
    {
        bool const success = adapter_bridge.initialize();
        if (!success) {
            return make_error("Failed to initialize Metal adapter");
        }

        metal_device = adapter_bridge.get_metal_device();
        return {};
    }

    void* get_metal_device() const
    {
        return metal_device;
    }

private:
    AdapterImplBridge adapter_bridge;
    void* metal_device = nullptr;
};

Adapter::Adapter(Instance const& gpu)
    : m_impl(make<Impl>(gpu))
{
}

Adapter::Adapter(Adapter&&) noexcept = default;
Adapter& Adapter::operator=(Adapter&&) noexcept = default;
Adapter::~Adapter() = default;

ErrorOr<void> Adapter::initialize()
{
    return m_impl->initialize();
}

}
