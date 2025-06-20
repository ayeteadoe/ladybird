/*
 * Copyright (c) 2018-2020, Andreas Kling <andreas@ladybird.org>
 * Copyright (c) 2021, Tobias Christiansen <tobyase@serenityos.org>
 * Copyright (c) 2021-2023, Sam Atkins <atkinssj@serenityos.org>
 * Copyright (c) 2022-2023, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/Color.h>
#include <LibWeb/CSS/CSSStyleValue.h>
#include <LibWeb/CSS/Length.h>

namespace Web::CSS {

enum class ShadowPlacement {
    Outer,
    Inner,
};

class ShadowStyleValue final : public StyleValueWithDefaultOperators<ShadowStyleValue> {
public:
    static ValueComparingNonnullRefPtr<ShadowStyleValue const> create(
        ValueComparingRefPtr<CSSStyleValue const> color,
        ValueComparingNonnullRefPtr<CSSStyleValue const> offset_x,
        ValueComparingNonnullRefPtr<CSSStyleValue const> offset_y,
        ValueComparingRefPtr<CSSStyleValue const> blur_radius,
        ValueComparingRefPtr<CSSStyleValue const> spread_distance,
        ShadowPlacement placement)
    {
        return adopt_ref(*new (nothrow) ShadowStyleValue(move(color), move(offset_x), move(offset_y), move(blur_radius), move(spread_distance), placement));
    }
    virtual ~ShadowStyleValue() override = default;

    ValueComparingNonnullRefPtr<CSSStyleValue const> color() const;
    ValueComparingNonnullRefPtr<CSSStyleValue const> offset_x() const { return m_properties.offset_x; }
    ValueComparingNonnullRefPtr<CSSStyleValue const> offset_y() const { return m_properties.offset_y; }
    ValueComparingNonnullRefPtr<CSSStyleValue const> blur_radius() const;
    ValueComparingNonnullRefPtr<CSSStyleValue const> spread_distance() const;
    ShadowPlacement placement() const { return m_properties.placement; }

    virtual String to_string(SerializationMode) const override;

    bool properties_equal(ShadowStyleValue const& other) const { return m_properties == other.m_properties; }

private:
    ShadowStyleValue(
        ValueComparingRefPtr<CSSStyleValue const> color,
        ValueComparingNonnullRefPtr<CSSStyleValue const> offset_x,
        ValueComparingNonnullRefPtr<CSSStyleValue const> offset_y,
        ValueComparingRefPtr<CSSStyleValue const> blur_radius,
        ValueComparingRefPtr<CSSStyleValue const> spread_distance,
        ShadowPlacement placement)
        : StyleValueWithDefaultOperators(Type::Shadow)
        , m_properties {
            .color = move(color),
            .offset_x = move(offset_x),
            .offset_y = move(offset_y),
            .blur_radius = move(blur_radius),
            .spread_distance = move(spread_distance),
            .placement = placement
        }
    {
    }

    virtual ValueComparingNonnullRefPtr<CSSStyleValue const> absolutized(CSSPixelRect const& viewport_rect, Length::FontMetrics const& font_metrics, Length::FontMetrics const& root_font_metrics) const override;

    struct Properties {
        ValueComparingRefPtr<CSSStyleValue const> color;
        ValueComparingNonnullRefPtr<CSSStyleValue const> offset_x;
        ValueComparingNonnullRefPtr<CSSStyleValue const> offset_y;
        ValueComparingRefPtr<CSSStyleValue const> blur_radius;
        ValueComparingRefPtr<CSSStyleValue const> spread_distance;
        ShadowPlacement placement;
        bool operator==(Properties const&) const = default;
    } m_properties;
};

}
