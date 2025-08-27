/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/Function.h>
#include <AK/Noncopyable.h>
#include <AK/NonnullOwnPtr.h>
#include <LibWebView/Export.h>

#if defined(AK_OS_WINDOWS)
class QAbstractNativeEventFilter;
#endif

namespace WebView {

class WEBVIEW_API TimeZoneWatcher {
    AK_MAKE_NONCOPYABLE(TimeZoneWatcher);

public:
    static ErrorOr<NonnullOwnPtr<TimeZoneWatcher>> create();
    virtual ~TimeZoneWatcher() = default;

    virtual void set_on_time_zone_changed(Function<void()> on_time_zone_changed)
    {
        this->on_time_zone_changed = move(on_time_zone_changed);
    }

#if defined(AK_OS_WINDOWS)
    virtual QAbstractNativeEventFilter& qt_native_event_filter() = 0;
#endif

protected:
    TimeZoneWatcher() = default;

    Function<void()> on_time_zone_changed;
};

}
