/*
 * Copyright (c) 2023, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/HashMap.h>
#include <AK/String.h>
#include <LibWeb/Forward.h>

namespace Web::HTML::TokenizedFeature {

#define TOKENIZED_FEATURE(Feature) \
    enum class Feature {           \
        Yes,                       \
        No,                        \
    }

TOKENIZED_FEATURE(Location);
TOKENIZED_FEATURE(Menubar);
TOKENIZED_FEATURE(NoOpener);
TOKENIZED_FEATURE(NoReferrer);
TOKENIZED_FEATURE(Popup);
TOKENIZED_FEATURE(Resizable);
TOKENIZED_FEATURE(Scrollbars);
TOKENIZED_FEATURE(Status);
TOKENIZED_FEATURE(Toolbar);

using Map = OrderedHashMap<String, String>;

}

namespace Web::HTML {

WEB_API TokenizedFeature::Map tokenize_open_features(StringView features);
WEB_API TokenizedFeature::Popup check_if_a_popup_window_is_requested(TokenizedFeature::Map const&);

template<Enum T>
T parse_boolean_feature(StringView value);

}
