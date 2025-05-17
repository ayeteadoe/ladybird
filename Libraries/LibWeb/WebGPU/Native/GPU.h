/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef EXPERIMENTAL_WEBGPU_WGPU_IMPL
#    include <LibWeb/WebGPU/Native/WGPU/GPUInstance.h>
#else
#    include <LibWeb/WebGPU/Native/Stub/GPUInstance.h>
#endif

namespace Web::WebGPU::Native {

#ifdef EXPERIMENTAL_WEBGPU_WGPU_IMPL
using GPU = Web::WebGPU::Native::WGPU::GPUInstance;
#else
using GPU = Web::WebGPU::Native::Stub::GPUInstance;
#endif

}
