/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <AK/NonnullRawPtr.h>
#include <AK/String.h>
#include <LibWebGPUNative/Export.h>
#include <LibWebGPUNative/ShaderModule.h>

namespace WebGPUNative {

class Device;

struct ProgrammableStage {
    NonnullRawPtr<ShaderModule> module;
    String entry_point;
};

struct VertexState : public ProgrammableStage {
};

struct FragmentState : public ProgrammableStage {
};

struct RenderPipelineDescriptor {
    VertexState vertex;
    Optional<FragmentState> fragment;
};

class WEBGPUNATIVE_API RenderPipeline {
public:
    friend class RenderPassEncoder;

    explicit RenderPipeline(Device const&, RenderPipelineDescriptor const&);
    RenderPipeline(RenderPipeline&&) noexcept;
    RenderPipeline& operator=(RenderPipeline&&) noexcept;
    ~RenderPipeline();

    ErrorOr<void> initialize();

private:
    struct Impl;
    NonnullOwnPtr<Impl> m_impl;
};

}
