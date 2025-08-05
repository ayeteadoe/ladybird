/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AK/ByteString.h"

#include <AK/Platform.h>
#include <AK/Windows.h>
#include <LibCore/TimeZoneWatcher.h>

#if !defined(AK_OS_WINDOWS)
static_assert(false, "This file must only be used for Windows");
#endif

namespace Core {

class TimeZoneWatcherImpl final : public TimeZoneWatcher {
public:
    static ErrorOr<NonnullOwnPtr<TimeZoneWatcherImpl>> create()
    {
        auto watcher = adopt_own(*new TimeZoneWatcherImpl());
        TRY(watcher->initialize());
        return watcher;
    }

    ~TimeZoneWatcherImpl() override
    {
        if (m_window) {
            DestroyWindow(m_window);
            m_window = nullptr;
        }
    }

private:
    TimeZoneWatcherImpl() = default;

    ErrorOr<void> initialize()
    {
        static constexpr auto window_class_name = L"TimeZoneWatcherWindow";

        WNDCLASSW wc = {};
        wc.lpfnWndProc = message_callback;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = window_class_name;

        if (RegisterClassW(&wc) == 0)
            return Error::from_windows_error();

        // https://learn.microsoft.com/en-us/windows/win32/winmsg/window-features#message-only-windows
        m_window = CreateWindowW(
            window_class_name,
            nullptr,
            0, 0, 0, 0, 0,
            HWND_MESSAGE,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr);

        if (m_window == nullptr)
            return Error::from_windows_error();

        SetWindowLongPtrW(m_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        return {};
    }

    static LRESULT CALLBACK message_callback(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
    {
        if (msg == WM_TIMECHANGE) {
            auto* watcher = reinterpret_cast<TimeZoneWatcherImpl*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if (watcher && watcher->on_time_zone_changed)
                watcher->on_time_zone_changed();
        }
        return DefWindowProcW(hwnd, msg, w, l);
    }

    HWND m_window { nullptr };
};

ErrorOr<NonnullOwnPtr<TimeZoneWatcher>> TimeZoneWatcher::create()
{
    return TimeZoneWatcherImpl::create();
}

}
