/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Web::WebGPU::Native {

// TODO: Make a CRTP impl here. This will be the equivalent to my WindowImpl abstraction

// FIXME: Formally  expose API required in spec
//  https://www.w3.org/TR/webgpu/#gpudevice
template<typename Impl>
class GPUDevice {
public:
    // TODO: create_buffer(), for this see if there is any benefit incorporating any of the VertexArray infra (e.g. stride calculations, etc)

    // TODO: create_pipeline_layout()

    // TODO: create_shader_module()

    // TODO: create_texture()

    // TODO: create_render_pipeline()

protected:
    GPUDevice() = default;
};

}
