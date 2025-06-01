/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWebGPUNative/Instance.h>
#include <vulkan/vulkan_core.h>

namespace WebGPUNative {

struct Instance::Impl {
    ~Impl();

    ErrorOr<void> initialize();

    VkInstance instance() const { return m_instance; }

private:
    VkInstance m_instance { VK_NULL_HANDLE };
};

}
