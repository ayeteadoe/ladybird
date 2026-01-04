/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <AK/NonnullOwnPtr.h>
#include <LibCore/EventLoopImplementation.h>

namespace Ladybird {

class EventLoopManagerWin32 final : public Core::EventLoopManager {
public:
    virtual NonnullOwnPtr<Core::EventLoopImplementation> make_implementation() override;

    virtual intptr_t register_timer(Core::EventReceiver&, int interval_milliseconds, bool should_reload) override;
    virtual void unregister_timer(intptr_t timer_id) override;

    virtual void register_notifier(Core::Notifier&) override;
    virtual void unregister_notifier(Core::Notifier&) override;

    virtual void did_post_event() override;

    virtual int register_signal(int, Function<void(int)>) override { VERIFY_NOT_REACHED(); }
    virtual void unregister_signal(int) override { VERIFY_NOT_REACHED(); }
};

class EventLoopImplementationWin32 final : public Core::EventLoopImplementation {
public:
    static NonnullOwnPtr<EventLoopImplementationWin32> create() { return adopt_own(*new EventLoopImplementationWin32); }

    virtual int exec() override;
    virtual size_t pump(PumpMode) override;
    virtual void quit(int) override;
    virtual void wake() override;
    virtual bool was_exit_requested() const override;

private:
    int m_exit_code { 0 };
    bool m_should_quit { false };
};

}
