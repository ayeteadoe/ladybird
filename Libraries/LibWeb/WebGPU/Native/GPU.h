/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/WebGPU/Native/GPUAdapter.h>

namespace Web::WebGPU::Native {

// FIXME: Formally expose API required in spec
//  https://developer.mozilla.org/en-US/docs/Web/API/GPU
template<typename Impl>
class GPU {
public:
    GPUAdapter<Impl> request_adapter() const
    {
        return static_cast<Impl*>(this)->request_adapter();
    }

protected:
    GPU() = default;
};

}
