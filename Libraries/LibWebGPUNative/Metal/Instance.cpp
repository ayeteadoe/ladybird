/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Instance.h>

namespace WebGPUNative {

struct Instance::Impl {
    ErrorOr<void> initialize() { return {}; }
};

Instance::Instance()
    : m_impl(make<Impl>())
{
}

Instance::~Instance() = default;

ErrorOr<void> Instance::initialize()
{
    return m_impl->initialize();
}

}
