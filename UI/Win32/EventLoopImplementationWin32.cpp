/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteString.h>
#include <AK/HashMap.h>
#include <AK/IDAllocator.h>
#include <AK/OwnPtr.h>
#include <AK/Traits.h>
#include <LibCore/Event.h>
#include <LibCore/EventReceiver.h>
#include <LibCore/Notifier.h>
#include <LibCore/ThreadEventQueue.h>
#include <LibThreading/RWLock.h>
#include <UI/Win32/EventLoopImplementationWin32.h>

#include <AK/Windows.h>

namespace Ladybird {

struct ThreadData;
static thread_local OwnPtr<ThreadData> s_this_thread_data;
static HashMap<pthread_t, ThreadData*> s_thread_data;
static thread_local Optional<pthread_t> s_thread_id;
static Threading::RWLock s_thread_data_lock;

enum {
    WM_LADYBIRD_WAKE = WM_USER,
    WM_LADYBIRD_SOCKETNOTIFIER = WM_USER + 1
};

struct TimerData {
    WeakPtr<Core::EventReceiver> weak_receiver;
    bool is_periodic { false };
    int id { 0 };
};

struct NotifierData {
    WeakPtr<Core::Notifier> weak_notifier;
};

LRESULT CALLBACK EventLoopWindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

struct ThreadData {
    ThreadData()
        : class_name(ByteString::formatted("EventLoopImplementation{}"sv, pthread_self().p))
    {
        WNDCLASS wnd_class;
        wnd_class.style = 0;
        wnd_class.lpfnWndProc = EventLoopWindowProc;
        wnd_class.cbClsExtra = 0;
        wnd_class.cbWndExtra = 0;
        wnd_class.hInstance = GetModuleHandle(nullptr);
        wnd_class.hIcon = nullptr;
        wnd_class.hCursor = 0;
        wnd_class.hbrBackground = 0;
        wnd_class.lpszMenuName = nullptr;
        wnd_class.lpszClassName = class_name.characters();

        atom = RegisterClass(&wnd_class);
        VERIFY(atom);

        hwnd = CreateWindow(class_name.characters(), // classname
            class_name.characters(),                 // window name
            0,                                       // style
            0, 0, 0, 0,                              // geometry
            HWND_MESSAGE,                            // parent
            nullptr,                                 // menu handle
            GetModuleHandle(nullptr),                // application
            nullptr);                                // windows creation data
        VERIFY(hwnd);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    ~ThreadData()
    {
        DestroyWindow(hwnd);
        UnregisterClass(class_name.characters(), GetModuleHandle(nullptr));
    }

    static ThreadData& the()
    {
        if (!s_thread_id.has_value())
            s_thread_id = pthread_self();
        if (!s_this_thread_data) {
            s_this_thread_data = make<ThreadData>();
            Threading::RWLockLocker<Threading::LockMode::Write> locker(s_thread_data_lock);
            s_thread_data.set(s_thread_id.value(), s_this_thread_data);
        }
        return *s_this_thread_data;
    }

    static ThreadData* for_thread(pthread_t thread_id)
    {
        Threading::RWLockLocker<Threading::LockMode::Read> locker(s_thread_data_lock);
        return s_thread_data.get(thread_id).value_or(nullptr);
    }

    ByteString class_name;
    ATOM atom;
    HWND hwnd;
    IDAllocator timer_id_allocator;
    HashMap<int, NonnullOwnPtr<TimerData>> timers;
    HashMap<int, NonnullOwnPtr<NotifierData>> notifiers;
};

NonnullOwnPtr<Core::EventLoopImplementation> EventLoopManagerWin32::make_implementation()
{
    return EventLoopImplementationWin32::create();
}

intptr_t EventLoopManagerWin32::register_timer(Core::EventReceiver& timer_event, int interval_milliseconds, bool should_reload)
{
    auto& thread_data = ThreadData::the();

    auto const timer_id = thread_data.timer_id_allocator.allocate();
    thread_data.timers.set(timer_id, make<TimerData>(timer_event.make_weak_ptr(), should_reload, timer_id));

    UINT_PTR timer_result = SetTimer(thread_data.hwnd, timer_id, interval_milliseconds, nullptr);
    VERIFY(static_cast<int>(timer_result) == timer_id);

    return timer_id;
}

void EventLoopManagerWin32::unregister_timer(intptr_t timer_id)
{
    auto& thread_data = ThreadData::the();
    auto maybe_timer_data = thread_data.timers.take(timer_id);
    if (!maybe_timer_data.has_value())
        return;
    auto timer_data = maybe_timer_data.release_value();
    BOOL result = KillTimer(thread_data.hwnd, timer_id);
    VERIFY(!timer_data->is_periodic || result);
}

static int notifier_type_to_network_event(Core::NotificationType type)
{
    switch (type) {
    case Core::NotificationType::Read:
        return FD_READ | FD_CLOSE | FD_ACCEPT;
    case Core::NotificationType::Write:
        return FD_WRITE;
    default:
        dbgln("This notification type is not implemented: {}", static_cast<int>(type));
        VERIFY_NOT_REACHED();
    }
}

void EventLoopManagerWin32::register_notifier(Core::Notifier& notifier)
{
    auto& thread_data = ThreadData::the();

    int const socket_fd = notifier.fd();
    thread_data.notifiers.set(socket_fd, make<NotifierData>(notifier.make_weak_ptr<Core::Notifier>()));

    int result = WSAAsyncSelect(socket_fd, thread_data.hwnd, WM_LADYBIRD_SOCKETNOTIFIER, notifier_type_to_network_event(notifier.type()));
    VERIFY(!result);
}

void EventLoopManagerWin32::unregister_notifier(Core::Notifier& notifier)
{
    auto* thread_data = ThreadData::for_thread(notifier.owner_thread());
    if (!thread_data)
        return;

    int const socket_fd = notifier.fd();
    auto maybe_notifier_data = thread_data->notifiers.take(socket_fd);
    if (!maybe_notifier_data.has_value())
        return;
    auto notifier_data = maybe_notifier_data.release_value();
    int result = WSAAsyncSelect(socket_fd, thread_data->hwnd, 0, 0);
    VERIFY(!result);
}

void EventLoopManagerWin32::did_post_event()
{
    auto& thread_data = ThreadData::the();
    PostMessage(thread_data.hwnd, WM_LADYBIRD_WAKE, 0, 0);
}

int EventLoopImplementationWin32::exec()
{
    while (!m_should_quit)
        pump(PumpMode::WaitForEvents);
    return m_exit_code;
}

size_t EventLoopImplementationWin32::pump(PumpMode pump_mode)
{
    MSG msg;
    BOOL result = false;
    if (pump_mode == PumpMode::WaitForEvents)
        result = GetMessage(&msg, nullptr, 0, 0);
    else
        result = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
    m_should_quit = msg.message == WM_QUIT;

    if (result) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return Core::ThreadEventQueue::current().process();
}

void EventLoopImplementationWin32::quit(int exit_code)
{
    PostQuitMessage(exit_code);
}

void EventLoopImplementationWin32::wake()
{
    auto& thread_data = ThreadData::the();
    PostMessage(thread_data.hwnd, WM_LADYBIRD_WAKE, 0, 0);
}

bool EventLoopImplementationWin32::was_exit_requested() const
{
    return m_should_quit;
}

LRESULT CALLBACK EventLoopWindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    auto thread_data = reinterpret_cast<ThreadData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (message) {
    case WM_TIMER: {
        // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-timer
        auto timer_id = static_cast<int>(w_param);
        if (auto timer_data = thread_data->timers.get(timer_id); timer_data.has_value()) {
            if (auto strong_receiver = timer_data.value()->weak_receiver.strong_ref())
                Core::ThreadEventQueue::current().post_event(strong_receiver, Core::Event::Type::Timer);
            if (!timer_data.value()->is_periodic) {
                BOOL result = KillTimer(hwnd, timer_data.value()->id);
                VERIFY(result);
            }
        }
        return 0;
    }
    case WM_LADYBIRD_SOCKETNOTIFIER: {
        auto socket_fd = static_cast<int>(w_param);
        if (auto notifier_data = thread_data->notifiers.get(socket_fd); notifier_data.has_value()) {
            if (auto strong_notifier = notifier_data.value()->weak_notifier.strong_ref())
                Core::ThreadEventQueue::current().post_event(strong_notifier, Core::Event::Type::NotifierActivation);
        }
        return 0;
    }
    case WM_LADYBIRD_WAKE:
    default:
        return DefWindowProc(hwnd, message, w_param, l_param);
    }
}

}

namespace AK {

template<>
struct Traits<pthread_t> : public DefaultTraits<pthread_t> {
    static unsigned hash(pthread_t thread_id)
    {
        return Traits<intptr_t>::hash(reinterpret_cast<intptr_t>(thread_id.p));
    }

    static constexpr bool equals(pthread_t a, pthread_t b)
    {
        return Traits<intptr_t>::equals(reinterpret_cast<intptr_t>(a.p), reinterpret_cast<intptr_t>(b.p));
    }
};

}
