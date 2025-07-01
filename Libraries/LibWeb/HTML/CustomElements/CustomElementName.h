/*
 * Copyright (c) 2023, Srikavin Ramkumar <me@srikavin.me>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/StringView.h>
#include <LibWeb/Forward.h>

namespace Web::HTML {

WEB_API bool is_valid_custom_element_name(StringView name);

}
