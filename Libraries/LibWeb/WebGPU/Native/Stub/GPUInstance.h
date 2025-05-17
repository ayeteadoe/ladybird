/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibCore/Promise.h>

namespace Web::WebGPU::Native::Stub {

class GPUAdapter;

class GPUInstance {
protected:
    AK::NonnullRefPtr<Core::Promise<GPUAdapter>> request_adapter();
};

}
