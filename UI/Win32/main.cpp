/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/EventLoop.h>
#include <LibCore/Socket.h>
#include <LibCore/System.h>
#include <LibCore/TimeZoneWatcher.h>
#include <LibCore/Timer.h>
#include <LibMain/Main.h>
#include <LibWebView/BrowserProcess.h>
#include <UI/Win32/Application.h>

#include <AK/Windows.h>

#include <CommCtrl.h>

struct WindowData {
    Core::TimeZoneWatcher& time_zone_watcher;
    HWND hwnd_tabcontrol { nullptr };
};

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    auto window_data = reinterpret_cast<WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (message) {
    case WM_TIMECHANGE: {
        window_data->time_zone_watcher.on_time_zone_changed();
        return 0;
    }
    case WM_SIZE: {
        int width = LOWORD(l_param);
        int height = HIWORD(l_param);
        // https://billthefarmer.github.io/blog/post/handling-resizing-in-windows/
        // TODO: Should instead just take in the actual Window instance and have a proper resize handler that propagates to child Windows to handle resizing relative to their given client area
        SetWindowPos(window_data->hwnd_tabcontrol, nullptr, 0, 0,
            width, height,
            SWP_NOMOVE | SWP_NOZORDER);
        return 0;
    }
    case WM_DESTROY: {
        Core::EventLoop::current().quit(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, message, w_param, l_param);
    }
}

ErrorOr<int> ladybird_main(Main::Arguments arguments)
{
    AK::set_rich_debug_enabled(true);

    auto app = TRY(Ladybird::Application::create(arguments));

    WebView::BrowserProcess browser_process;

    if (auto const& browser_options = WebView::Application::browser_options(); !browser_options.headless_mode.has_value()) {
        if (browser_options.force_new_process == WebView::ForceNewProcess::No) {
            auto disposition = TRY(browser_process.connect(browser_options.raw_urls, browser_options.new_window));

            if (disposition == WebView::BrowserProcess::ProcessDisposition::ExitProcess) {
                outln("Opening in existing process");
                return 0;
            }
        }

        browser_process.on_new_tab = [&]([[maybe_unused]] auto const& raw_urls) {
            // TODO: Should create new tab in the active Window
        };

        browser_process.on_new_window = [&]([[maybe_unused]] auto const& raw_urls) {
            // TODO: We should be able to create multiple Window for a single application, when any of them are activated (i.e. switched to) we should update the active window in Application
        };

        // TODO: Ask Application to create a Window with the initial raw URLs. This will Ultimately result in WebContent being spawned and us rendering the initial bitmap from the WebContentClient
        // browser_options.raw_urls
    }

    // https://learn.microsoft.com/en-us/windows/win32/controls/create-a-tab-control-in-the-main-window
    // TODO:
    //  - The main Window class should have a tab control hwnd.
    //  - Then a Tab class should have a child hwnd for general tab content display. For example, the search/URL bar will be at the top, followed by the actual web content
    //  - Then a WebContentView class should have a child hwnd that renders the bitmap from the WebContentClient
    //  - Parenting: Application -> Window -> Tab -> WebContentView

    // TODO: This should go into a Window class with a .h/.cpp, our Application instance will create the Window
    static constexpr auto s_class_name = "LadybirdMainWindow";
    static auto s_h_instance = GetModuleHandle(nullptr);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = s_h_instance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = s_class_name;
    wcex.hIconSm = nullptr;

    RegisterClassEx(&wcex);

    HWND hwnd_window = CreateWindow("LadybirdMainWindow", "Ladybird", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, nullptr, s_h_instance, nullptr);
    VERIFY(hwnd_window);
    // TODO: Use SetWindowLongPtr() to associated our Ladybird::Application with our MainWindowProc

    // TODO: This should go into a Tab class with a .h/.cpp
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rect;
    BOOL result = GetClientRect(hwnd_window, &rect);
    VERIFY(result);
    HWND hwnd_tabcontrol = CreateWindow(WC_TABCONTROL, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, rect.right, rect.bottom,
        hwnd_window, NULL, s_h_instance, NULL);
    VERIFY(hwnd_tabcontrol);

    SendMessage(hwnd_tabcontrol, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)true);

    TCITEM tabcontrol_item = {};
    tabcontrol_item.mask = TCIF_TEXT;
    CHAR new_tab_text[] = "New Tab";
    tabcontrol_item.pszText = new_tab_text;

    int tabcontrol_index = 0;
    int rc = TabCtrl_InsertItem(hwnd_tabcontrol, tabcontrol_index, &tabcontrol_item);
    VERIFY(rc == tabcontrol_index);

    auto window_data = make<WindowData>(app->time_zone_watcher().value(), hwnd_tabcontrol);
    SetWindowLongPtr(hwnd_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window_data.ptr()));

    ShowWindow(hwnd_window, SW_SHOWDEFAULT);
    UpdateWindow(hwnd_window);

    // TODO: Remove this event loop implementation testing stuff
    int socket_fds[2] {};
    TRY(Core::System::socketpair(AF_LOCAL, SOCK_STREAM, 0, socket_fds));

    auto socket_read = TRY(Core::LocalSocket::adopt_fd(socket_fds[0]));
    socket_read->on_ready_to_read = [&socket_read]() {
        static constexpr size_t buffer_size = 256;
        static char buffer[buffer_size];
        auto bytes = socket_read->read_some({ buffer, buffer_size });
        if (!bytes.is_error()) {
            dbgln("{}", ByteString(reinterpret_cast<char const*>(bytes.value().data()), bytes.value().size()));
        }
    };

    auto timer = Core::Timer::create_repeating(1000, [&socket_fds] {
        static int timer_count = 0;
        auto message = ByteString::formatted("Timer tick {}", ++timer_count);
        [[maybe_unused]] auto result = Core::System::send(socket_fds[1], message.bytes(), 0);
    });
    timer->start();

    return app->execute();
}
