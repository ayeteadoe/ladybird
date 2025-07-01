/*
 * Copyright (c) 2020, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Forward.h>
#include <LibWeb/Geometry/DOMPoint.h>
#include <LibWeb/SVG/SVGGraphicsElement.h>

namespace Web::SVG {

// https://svgwg.org/svg2-draft/types.html#InterfaceSVGGeometryElement
class WEB_API SVGGeometryElement : public SVGGraphicsElement {
    WEB_PLATFORM_OBJECT(SVGGeometryElement, SVGGraphicsElement);

public:
    virtual GC::Ptr<Layout::Node> create_layout_node(GC::Ref<CSS::ComputedProperties>) override;

    virtual Gfx::Path get_path(CSSPixelSize viewport_size) = 0;

    float get_total_length();
    GC::Ref<Geometry::DOMPoint> get_point_at_length(float distance);

protected:
    SVGGeometryElement(DOM::Document& document, DOM::QualifiedName qualified_name);

    virtual void initialize(JS::Realm&) override;
};

}
