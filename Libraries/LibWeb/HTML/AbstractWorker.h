/*
 * Copyright (c) 2024, Jamie Mansfield <jmansfield@cadixdev.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/Forward.h>

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/workers.html#abstractworker
class WEB_API AbstractWorker {
public:
    virtual ~AbstractWorker() = default;

    WebIDL::CallbackType* onerror();
    void set_onerror(WebIDL::CallbackType*);

protected:
    virtual DOM::EventTarget& this_event_target() = 0;
};

}
