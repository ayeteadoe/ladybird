/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullOwnPtr.h>
#include <LibCore/Promise.h>

namespace WebGPUNative {

class Adapter;

class Instance {
public:
    friend class Adapter;

    explicit Instance();
    Instance(Instance&&) noexcept;
    Instance& operator=(Instance&&) noexcept;
    ~Instance();

    static bool supported();

    ErrorOr<void> initialize();

    Adapter adapter() const;

    NonnullRefPtr<Core::Promise<Adapter>> request_adapter();

private:
    struct Impl;
    NonnullOwnPtr<Impl> m_impl;
};

}
