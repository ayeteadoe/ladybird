/*
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <LibWeb/Forward.h>

namespace Web::Infra {

WEB_API void byte_lowercase(ByteBuffer&);
WEB_API void byte_uppercase(ByteBuffer&);
WEB_API bool is_prefix_of(ReadonlyBytes potential_prefix, ReadonlyBytes input);
WEB_API bool is_byte_less_than(ReadonlyBytes a, ReadonlyBytes b);

}
