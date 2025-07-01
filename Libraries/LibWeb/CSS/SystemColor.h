/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/Color.h>
#include <LibWeb/CSS/PreferredColorScheme.h>
#include <LibWeb/Forward.h>

// https://www.w3.org/TR/css-color-4/#css-system-colors
namespace Web::CSS::SystemColor {

WEB_API Color accent_color(PreferredColorScheme);
WEB_API Color accent_color_text(PreferredColorScheme);
WEB_API Color active_text(PreferredColorScheme);
WEB_API Color button_border(PreferredColorScheme);
WEB_API Color button_face(PreferredColorScheme);
WEB_API Color button_text(PreferredColorScheme);
WEB_API Color canvas(PreferredColorScheme);
WEB_API Color canvas_text(PreferredColorScheme);
WEB_API Color field(PreferredColorScheme);
WEB_API Color field_text(PreferredColorScheme);
WEB_API Color gray_text(PreferredColorScheme);
WEB_API Color highlight(PreferredColorScheme);
WEB_API Color highlight_text(PreferredColorScheme);
WEB_API Color link_text(PreferredColorScheme);
WEB_API Color mark(PreferredColorScheme);
WEB_API Color mark_text(PreferredColorScheme);
WEB_API Color selected_item(PreferredColorScheme);
WEB_API Color selected_item_text(PreferredColorScheme);
WEB_API Color visited_text(PreferredColorScheme);

}
