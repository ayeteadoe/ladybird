/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Instance.h>
#include <d3d12sdklayers.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace WebGPUNative {

struct Instance::Impl {
    ErrorOr<void> initialize()
    {

#ifdef WEBGPUNATIVE_DEBUG
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
        }
#endif
        return {};
    }
};

Instance::Instance()
    : m_impl(make<Impl>())
{
}

Instance::~Instance() = default;

ErrorOr<void> Instance::initialize()
{
    return m_impl->initialize();
}

}
