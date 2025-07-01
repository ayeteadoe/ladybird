/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibWeb/Forward.h>

namespace Web::SRI {

// https://w3c.github.io/webappsec-subresource-integrity/#integrity-metadata
struct Metadata {
    String algorithm;    // "alg"
    String base64_value; // "val"
    String options {};   // "opt"
};

WEB_API ErrorOr<String> apply_algorithm_to_bytes(StringView algorithm, ByteBuffer const& bytes);
WEB_API ErrorOr<Vector<Metadata>> parse_metadata(StringView metadata);
WEB_API ErrorOr<Vector<Metadata>> get_strongest_metadata_from_set(Vector<Metadata> const& set);
WEB_API ErrorOr<bool> do_bytes_match_metadata_list(ByteBuffer const& bytes, StringView metadata_list);

}
