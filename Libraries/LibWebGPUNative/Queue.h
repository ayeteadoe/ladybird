/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <AK/NonnullRawPtr.h>
#include <LibWebGPUNative/Forward.h>

namespace WebGPUNative {

class Buffer;
class Device;
class CommandBuffer;

class WEBGPUNATIVE_API Queue {
public:
    explicit Queue(Device const&);
    Queue(Queue&&) noexcept;
    Queue& operator=(Queue&&) noexcept;
    ~Queue();

    ErrorOr<void> submit(Vector<NonnullRawPtr<CommandBuffer>> const&);
    void on_submitted(Function<void()> callback);

    ErrorOr<void> write_buffer(NonnullRawPtr<Buffer> const&, size_t, Vector<float> const&, size_t, size_t);

private:
    struct Impl;
    NonnullOwnPtr<Impl> m_impl;
};

}
