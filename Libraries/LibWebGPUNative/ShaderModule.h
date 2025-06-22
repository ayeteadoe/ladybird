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

struct ShaderModuleDescriptor {
    String code;
};

class WEBGPUNATIVE_API ShaderModule {
public:
    explicit ShaderModule(Device const&, ShaderModuleDescriptor const&);
    ShaderModule(ShaderModule&&) noexcept;
    ShaderModule& operator=(ShaderModule&&) noexcept;
    ~ShaderModule();

    ErrorOr<void> initialize();

private:
    struct Impl;
    NonnullOwnPtr<Impl> m_impl;
};

}
