/*
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/CSS/EdgeRect.h>
#include <LibWeb/Forward.h>

namespace Web::CSS {

class WEB_API Clip {
public:
    enum class Type {
        Auto,
        Rect
    };

    Clip(Type type, EdgeRect edge_rect);
    Clip(EdgeRect edge_rect);

    static Clip make_auto();

    bool is_auto() const { return m_type == Type::Auto; }
    bool is_rect() const { return m_type == Type::Rect; }

    EdgeRect to_rect() const { return m_edge_rect; }

private:
    Type m_type;
    EdgeRect m_edge_rect;
};

}
