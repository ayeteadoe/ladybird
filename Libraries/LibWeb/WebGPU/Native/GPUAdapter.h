/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef EXPERIMENTAL_WEBGPU_WGPU_IMPL
#    include <LibWeb/WebGPU/Native/WGPU/GPUAdapter.h>
#else
#    include <LibWeb/WebGPU/Native/Stub/GPUAdapter.h>
#endif

namespace Web::WebGPU::Native {
#ifdef EXPERIMENTAL_WEBGPU_WGPU_IMPL
using GPUAdapter = Web::WebGPU::Native::WGPU::GPUAdapter;
#else
using GPUAdapter = Web::WebGPU::Native::Stub::GPUAdapter;
#endif
}
