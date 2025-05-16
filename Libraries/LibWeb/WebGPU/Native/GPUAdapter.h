/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Web::WebGPU::Native {

// FIXME: Formally expose API required in spec
//  https://developer.mozilla.org/en-US/docs/Web/API/GPU
template<typename Impl>
class GPUAdapter {
public:
    // TODO: request_device()

    // void print_info() const;

protected:
    GPUAdapter() = default;
};

}
