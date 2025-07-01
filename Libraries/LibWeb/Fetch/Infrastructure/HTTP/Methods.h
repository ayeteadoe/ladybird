/*
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <LibWeb/Forward.h>

namespace Web::Fetch::Infrastructure {

[[nodiscard]] WEB_API bool is_method(ReadonlyBytes);
[[nodiscard]] WEB_API bool is_cors_safelisted_method(ReadonlyBytes);
[[nodiscard]] WEB_API bool is_forbidden_method(ReadonlyBytes);
[[nodiscard]] WEB_API ByteBuffer normalize_method(ReadonlyBytes);

}
