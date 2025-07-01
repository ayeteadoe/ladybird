/*
 * Copyright (c) 2023, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Span.h>
#include <AK/Vector.h>
#include <LibGfx/Color.h>
#include <LibGfx/Gradients.h>
#include <LibWeb/Forward.h>
#include <LibWeb/Painting/GradientData.h>
#include <LibWeb/Painting/PaintContext.h>

namespace Web::Painting {

WEB_API LinearGradientData resolve_linear_gradient_data(Layout::NodeWithStyle const&, CSSPixelSize, CSS::LinearGradientStyleValue const&);
WEB_API ConicGradientData resolve_conic_gradient_data(Layout::NodeWithStyle const&, CSS::ConicGradientStyleValue const&);
WEB_API RadialGradientData resolve_radial_gradient_data(Layout::NodeWithStyle const&, CSSPixelSize, CSS::RadialGradientStyleValue const&);

}
