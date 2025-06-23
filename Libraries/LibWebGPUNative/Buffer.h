/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <AK/String.h>
#include <LibWebGPUNative/Export.h>

namespace WebGPUNative {

class Device;

struct BufferDescriptor {
    size_t size;
};

class WEBGPUNATIVE_API Buffer {
public:
    friend class RenderPassEncoder;
    friend class RenderPipeline;
    friend class Queue;

    explicit Buffer(Device const&, BufferDescriptor const&);
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(Buffer&&) noexcept;
    ~Buffer();

    ErrorOr<void> initialize();

private:
    struct Impl;
    NonnullOwnPtr<Impl> m_impl;
};

}
