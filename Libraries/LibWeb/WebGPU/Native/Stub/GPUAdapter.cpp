/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <LibWeb/WebGPU/Native/Stub/GPUAdapter.h>
// #include <LibWeb/WebGPU/Native/Stub/GPUDevice.h>

#include <AK/Format.h>

namespace Web::WebGPU::Native::Stub {

void GPUAdapter::print_info() const
{
    AK::warnln("Adapter Stub: No information");
}

}
