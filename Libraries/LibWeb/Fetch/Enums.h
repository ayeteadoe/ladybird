/*
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Requests.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Responses.h>
#include <LibWeb/Forward.h>

namespace Web::Fetch {

[[nodiscard]] WEB_API ReferrerPolicy::ReferrerPolicy from_bindings_enum(Bindings::ReferrerPolicy);
[[nodiscard]] WEB_API Infrastructure::Request::Mode from_bindings_enum(Bindings::RequestMode);
[[nodiscard]] WEB_API Infrastructure::Request::CredentialsMode from_bindings_enum(Bindings::RequestCredentials);
[[nodiscard]] WEB_API Infrastructure::Request::CacheMode from_bindings_enum(Bindings::RequestCache);
[[nodiscard]] WEB_API Infrastructure::Request::RedirectMode from_bindings_enum(Bindings::RequestRedirect);
[[nodiscard]] WEB_API Infrastructure::Request::Priority from_bindings_enum(Bindings::RequestPriority);

[[nodiscard]] WEB_API Bindings::ReferrerPolicy to_bindings_enum(ReferrerPolicy::ReferrerPolicy);
[[nodiscard]] WEB_API Bindings::RequestDestination to_bindings_enum(Optional<Infrastructure::Request::Destination> const&);
[[nodiscard]] WEB_API Bindings::RequestMode to_bindings_enum(Infrastructure::Request::Mode);
[[nodiscard]] WEB_API Bindings::RequestCredentials to_bindings_enum(Infrastructure::Request::CredentialsMode);
[[nodiscard]] WEB_API Bindings::RequestCache to_bindings_enum(Infrastructure::Request::CacheMode);
[[nodiscard]] WEB_API Bindings::RequestRedirect to_bindings_enum(Infrastructure::Request::RedirectMode);
[[nodiscard]] WEB_API Bindings::ResponseType to_bindings_enum(Infrastructure::Response::Type);

}
