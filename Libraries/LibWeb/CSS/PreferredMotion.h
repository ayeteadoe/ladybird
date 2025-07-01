/*
 * Copyright (c) 2024, the Ladybird developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/StringView.h>
#include <LibWeb/Forward.h>

namespace Web::CSS {

enum class PreferredMotion {
    Auto,
    NoPreference,
    Reduce,
};

WEB_API PreferredMotion preferred_motion_from_string(StringView);
WEB_API StringView preferred_motion_to_string(PreferredMotion);

}
