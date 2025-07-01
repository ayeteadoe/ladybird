/*
 * Copyright (c) 2024, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Forward.h>
#include <LibWeb/Forward.h>

namespace Web::WebIDL {

WEB_API void log_trace_impl(JS::VM&, char const*);

WEB_API void log_trace(JS::VM& vm, char const* function);

WEB_API void set_enable_idl_tracing(bool enabled);

}
