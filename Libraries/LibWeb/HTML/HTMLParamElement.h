/*
 * Copyright (c) 2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Forward.h>
#include <LibWeb/HTML/HTMLElement.h>

namespace Web::HTML {

class WEB_API HTMLParamElement final : public HTMLElement {
    WEB_PLATFORM_OBJECT(HTMLParamElement, HTMLElement);
    GC_DECLARE_ALLOCATOR(HTMLParamElement);

public:
    virtual ~HTMLParamElement() override;

private:
    HTMLParamElement(DOM::Document&, DOM::QualifiedName);

    virtual void initialize(JS::Realm&) override;
};

}
