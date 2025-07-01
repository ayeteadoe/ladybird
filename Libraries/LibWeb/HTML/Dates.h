/*
 * Copyright (c) 2023, stelar7 <dudedbz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <AK/String.h>
#include <LibJS/Runtime/Date.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::HTML {

WEB_API u32 week_number_of_the_last_day(u64 year);
WEB_API bool is_valid_week_string(StringView value);
WEB_API bool is_valid_month_string(StringView value);
WEB_API bool is_valid_date_string(StringView value);
WEB_API bool is_valid_local_date_and_time_string(StringView value);
WEB_API String normalize_local_date_and_time_string(String const& value);
WEB_API bool is_valid_time_string(StringView value);
WEB_API WebIDL::ExceptionOr<GC::Ref<JS::Date>> parse_time_string(JS::Realm& realm, StringView value);

struct YearAndMonth {
    u32 year;
    u32 month;
};
WEB_API Optional<YearAndMonth> parse_a_month_string(StringView);

struct WeekYearAndWeek {
    u32 week_year;
    u32 week;
};
WEB_API Optional<WeekYearAndWeek> parse_a_week_string(StringView);

struct YearMonthDay {
    u32 year;
    u32 month;
    u32 day;
};

struct HourMinuteSecond {
    i32 hour;
    i32 minute;
    f32 second;
};

struct DateAndTime {
    YearMonthDay date;
    HourMinuteSecond time;
};

WEB_API Optional<YearMonthDay> parse_a_date_string(StringView);

WEB_API Optional<DateAndTime> parse_a_local_date_and_time_string(StringView);

WEB_API i32 number_of_months_since_unix_epoch(YearAndMonth);

}
