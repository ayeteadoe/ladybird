/*
 * Copyright (c) 2024, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Forward.h>

namespace Web::WebIDL {

void log_trace_impl(JS::VM&, char const*);

void log_trace(JS::VM& vm, char const* function);

void set_enable_idl_tracing(bool enabled);

}
