/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Assertions.h>

#include <UI/Qt/WebGPUTestBed/WebGPUWindow.h>

#include <QGuiApplication>
#include <QScreen>
#include <qpa/qplatformnativeinterface.h>

namespace Ladybird::WebGPUTestBed {

WebGPUWindow::WebGPUWindow(QWindow* parent)
    : QWindow(parent)
{
}

void WebGPUWindow::initialize()
{
    create_surface();
    VERIFY(m_surface.has_value());

    m_instance.request_adapter(m_surface.value(),
        [this](Web::WebGPU::Native::WGPU::GPUAdapter adapter) {
            adapter.print_info();

            adapter.request_device([this]([[maybe_unused]] Web::WebGPU::Native::WGPU::GPUDevice device) {
                // TODO: Create shader module from embedded WGSL shader resource
            });
        });
}

void WebGPUWindow::showEvent(QShowEvent* event)
{
    QWindow::showEvent(event);
    initialize();
}

void WebGPUWindow::resizeEvent(QResizeEvent* event)
{
    QWindow::resizeEvent(event);
    // TODO: update surface configuration every resize event
}

void WebGPUWindow::create_surface()
{
#if defined(Q_OS_MACOS)
    {
        // FIXME: Make Metal surface
    }
#elif defined(Q_OS_LINUX)
    {
        using namespace QNativeInterface;
        QPlatformNativeInterface* native = QGuiApplication::platformNativeInterface();
        VERIFY(native != nullptr);
        if (QGuiApplication::platformName().startsWith("wayland")) {
            create_wayland_surface(*native);
        } else if (QGuiApplication::platformName() == "xcb") {
            // FIXME: Make X11 surface
        }
    }
#elif defined(Q_OS_WIN)
    {
        // FIXME: Make Windows surface
    }
#else
#endif
}

void WebGPUWindow::create_wayland_surface(QPlatformNativeInterface& native_iface)
{
    struct wl_display* display = static_cast<struct wl_display*>(
        native_iface.nativeResourceForIntegration("display"));
    VERIFY(display != nullptr);

    struct wl_surface* surface = static_cast<struct wl_surface*>(
        native_iface.nativeResourceForWindow("surface", this));
    VERIFY(surface != nullptr);

    m_surface = m_instance.create_surface(display, surface);
}

}
