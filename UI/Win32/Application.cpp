/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <UI/Win32/Application.h>
#include <UI/Win32/EventLoopImplementationWin32.h>

namespace Ladybird {

Application::Application()
    : m_h_instance(GetModuleHandle(nullptr))
{
}

NonnullOwnPtr<Core::EventLoop> Application::create_platform_event_loop()
{
    if (!browser_options().headless_mode.has_value()) {
        Core::EventLoopManager::install(*new EventLoopManagerWin32);
    }
    return WebView::Application::create_platform_event_loop();
}

}
