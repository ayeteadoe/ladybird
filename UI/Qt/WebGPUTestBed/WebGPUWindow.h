/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUAdapter.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUDevice.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUInstance.h>
#include <LibWeb/WebGPU/Native/WGPU/GPUSurface.h>
#include <QWindow>

class QPlatformNativeInterface;

namespace Ladybird::WebGPUTestBed {

class WebGPUWindow final : public QWindow {

public:
    explicit WebGPUWindow(QWindow* parent = nullptr);

    void initialize();

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void create_surface();
    void create_wayland_surface(QPlatformNativeInterface& native_iface);

    Web::WebGPU::Native::WGPU::GPUInstance m_instance;
    AK::Optional<Web::WebGPU::Native::WGPU::GPUSurface> m_surface;
    AK::Optional<Web::WebGPU::Native::WGPU::GPUAdapter> m_adapter;
    AK::Optional<Web::WebGPU::Native::WGPU::GPUDevice> m_device;
};

}
