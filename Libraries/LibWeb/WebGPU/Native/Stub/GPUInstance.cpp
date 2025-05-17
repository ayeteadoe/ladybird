/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <AK/Assertions.h>
#include <LibWeb/WebGPU/Native/Stub/GPUAdapter.h>
#include <LibWeb/WebGPU/Native/Stub/GPUInstance.h>

namespace Web::WebGPU::Native::Stub {

AK::NonnullRefPtr<Core::Promise<GPUAdapter>> GPUInstance::request_adapter()
{
    return MUST(Core::Promise<GPUAdapter>::try_create());
}

}
