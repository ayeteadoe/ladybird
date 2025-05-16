/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/GPU.h>

namespace Web::WebGPU {

GPU::GPU(JS::Realm& realm)
    : Bindings::PlatformObject(realm)
{
}

GPU::~GPU() = default;

}
