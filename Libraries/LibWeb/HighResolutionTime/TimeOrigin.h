/*
 * Copyright (c) 2022, Andreas Kling <andreas@ladybird.org>
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Forward.h>
#include <LibWeb/Forward.h>
#include <LibWeb/HighResolutionTime/DOMHighResTimeStamp.h>

namespace Web::HighResolutionTime {

WEB_API DOMHighResTimeStamp estimated_monotonic_time_of_the_unix_epoch();
WEB_API DOMHighResTimeStamp get_time_origin_timestamp(JS::Object const&);
WEB_API DOMHighResTimeStamp coarsen_time(DOMHighResTimeStamp timestamp, bool cross_origin_isolated_capability = false);
WEB_API DOMHighResTimeStamp current_high_resolution_time(JS::Object const&);
WEB_API DOMHighResTimeStamp relative_high_resolution_time(DOMHighResTimeStamp, JS::Object const&);
WEB_API DOMHighResTimeStamp relative_high_resolution_coarsen_time(DOMHighResTimeStamp, JS::Object const&);
WEB_API DOMHighResTimeStamp coarsened_shared_current_time(bool cross_origin_isolated_capability = false);
WEB_API DOMHighResTimeStamp wall_clock_unsafe_current_time();
WEB_API DOMHighResTimeStamp unsafe_shared_current_time();

}
