/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Platform.h>
#include <AK/Windows.h>
#include <LibWebView/TimeZoneWatcher.h>

#include <QAbstractNativeEventFilter>
#include <QByteArray>

#if !defined(AK_OS_WINDOWS)
static_assert(false, "This file must only be used for Windows");
#endif

namespace WebView {

class TimeChangeEventFilter : public QAbstractNativeEventFilter {
public:
    bool nativeEventFilter(QByteArray const& event_type, void* message, qintptr*) override
    {
        if (event_type == QByteArrayLiteral("windows_generic_MSG")) {
            auto msg = static_cast<MSG*>(message);
            if (msg->message == WM_TIMECHANGE && on_time_zone_changed) {
                on_time_zone_changed();
            }
        }
        return false;
    }

    Function<void()> on_time_zone_changed;
};

class TimeZoneWatcherImpl final : public TimeZoneWatcher {
public:
    static ErrorOr<NonnullOwnPtr<TimeZoneWatcherImpl>> create()
    {
        auto watcher = adopt_own(*new TimeZoneWatcherImpl());
        TRY(watcher->initialize());
        return watcher;
    }

    QAbstractNativeEventFilter& qt_native_event_filter() override
    {
        return m_time_change_event_filter;
    }

protected:
    void set_on_time_zone_changed(Function<void()> on_time_zone_changed) override
    {
        m_time_change_event_filter.on_time_zone_changed = move(on_time_zone_changed);
    }

private:
    TimeZoneWatcherImpl() = default;

    ErrorOr<void> initialize()
    {
        return {};
    }

    TimeChangeEventFilter m_time_change_event_filter;
};

ErrorOr<NonnullOwnPtr<TimeZoneWatcher>> TimeZoneWatcher::create()
{
    return TimeZoneWatcherImpl::create();
}

}
