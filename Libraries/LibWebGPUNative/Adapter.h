/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>

namespace WebGPUNative {

class Instance;

class Adapter {
public:
    friend class Device;

    explicit Adapter(Instance const&);
    Adapter(Adapter&&) noexcept;
    Adapter& operator=(Adapter&&) noexcept;
    ~Adapter();

    ErrorOr<void> initialize();

private:
    struct Impl;
    NonnullOwnPtr<Impl> m_impl;
};

}
