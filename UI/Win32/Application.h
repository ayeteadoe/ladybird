/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWebView/Application.h>

#include <AK/Windows.h>

namespace Ladybird {

class Application final : public WebView::Application {
    WEB_VIEW_APPLICATION(Application)

private:
    explicit Application();

    virtual NonnullOwnPtr<Core::EventLoop> create_platform_event_loop() override;

    HINSTANCE m_h_instance { nullptr };
};

}
