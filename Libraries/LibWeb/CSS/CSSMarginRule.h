/*
 * Copyright (c) 2025, Sam Atkins <sam@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/CSS/CSSRule.h>
#include <LibWeb/Forward.h>

namespace Web::CSS {

// https://drafts.csswg.org/cssom/#cssmarginrule
class WEB_API CSSMarginRule final : public CSSRule {
    WEB_PLATFORM_OBJECT(CSSMarginRule, CSSRule);
    GC_DECLARE_ALLOCATOR(CSSMarginRule);

public:
    [[nodiscard]] static GC::Ref<CSSMarginRule> create(JS::Realm&, FlyString name, GC::Ref<CSSStyleProperties>);

    virtual ~CSSMarginRule() override = default;

    String name() const { return m_name.to_string(); }
    GC::Ref<CSSStyleProperties> style() { return m_style; }
    GC::Ref<CSSStyleProperties const> style() const { return m_style; }

private:
    CSSMarginRule(JS::Realm&, FlyString name, GC::Ref<CSSStyleProperties>);

    virtual void initialize(JS::Realm&) override;
    virtual String serialized() const override;
    virtual void visit_edges(Visitor&) override;

    FlyString m_name;
    GC::Ref<CSSStyleProperties> m_style;
};

WEB_API bool is_margin_rule_name(StringView);

}
