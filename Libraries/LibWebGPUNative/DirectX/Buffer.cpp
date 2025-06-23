/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/Buffer.h>
#include <LibWebGPUNative/DirectX/BufferImpl.h>

namespace WebGPUNative {

Buffer::Buffer(Device const& gpu_device, BufferDescriptor const& gpu_buffer_descriptor)
    : m_impl(make<Impl>(gpu_device, gpu_buffer_descriptor))
{
}

Buffer::Buffer(Buffer&&) noexcept = default;
Buffer& Buffer::operator=(Buffer&&) noexcept = default;
Buffer::~Buffer() = default;

ErrorOr<void> Buffer::initialize()
{
    return m_impl->initialize();
}

}
