/*
 * Copyright (c) 2023, Andreas Kling <andreas@ladybird.org>
 * Copyright (c) 2024-2025, stasoid <stasoid@yahoo.com>
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 * Copyright (c) 2025, Ryszard Goc <ryszardgoc@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Assertions.h>
#include <AK/Diagnostics.h>
#include <AK/HashMap.h>
#include <AK/NeverDestroyed.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/Windows.h>
#include <LibCore/EventLoopImplementationWindows.h>
#include <LibCore/Notifier.h>
#include <LibCore/ThreadEventQueue.h>
#include <LibCore/Timer.h>
#include <LibSync/Mutex.h>
#include <LibSync/MutexProtected.h>
#include <LibSync/Once.h>
#include <LibSync/RWLock.h>
#include <pthread.h>

struct OwnHandle {
    HANDLE handle = NULL;

    explicit OwnHandle(HANDLE h = NULL)
        : handle(h)
    {
    }

    OwnHandle(OwnHandle&& h)
    {
        handle = h.handle;
        h.handle = NULL;
    }

    // This operation can only be done when handle is NULL
    OwnHandle& operator=(OwnHandle&& other)
    {
        VERIFY(!handle);
        if (this == &other)
            return *this;
        handle = other.handle;
        other.handle = NULL;
        return *this;
    }

    ~OwnHandle()
    {
        if (handle)
            CloseHandle(handle);
    }

    bool operator==(OwnHandle const& h) const { return handle == h.handle; }
    bool operator==(HANDLE h) const { return handle == h; }
};

template<>
struct Traits<OwnHandle> : DefaultTraits<OwnHandle> {
    static unsigned hash(OwnHandle const& h) { return Traits<HANDLE>::hash(h.handle); }
};
template<>
constexpr bool IsHashCompatible<HANDLE, OwnHandle> = true;

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

namespace Core {

enum class CompletionType : u8 {
    Wake,
    Timer,
    Notifer,
    Process,
};

struct CompletionPacket {
    CompletionType type;
};

struct EventLoopWake final : CompletionPacket {
    OwnHandle wait_packet;
    OwnHandle wait_event;
};

struct EventLoopTimer final : CompletionPacket {

    ~EventLoopTimer()
    {
        CancelWaitableTimer(timer.handle);
    }

    OwnHandle timer;
    OwnHandle wait_packet;
    bool is_periodic;
    WeakPtr<EventReceiver> owner;
    pthread_t owner_thread { };
};

struct EventLoopNotifier final : CompletionPacket {

    ~EventLoopNotifier()
    {
    }

    WeakPtr<EventReceiver> notifier;
    OwnHandle wait_packet;
    OwnHandle wait_event;
};

struct EventLoopProcess final : CompletionPacket {
    ~EventLoopProcess() = default;

    OwnHandle process;
    pid_t pid;
    Function<void(pid_t)> exit_handler;
    OwnHandle jobobject;
};

struct ThreadData;

thread_local ThreadData* s_this_thread_data;
static pthread_key_t s_this_thread_data_key;

static void destroy_thread_data(void*);

static auto& thread_data()
{
    static NeverDestroyed<HashMap<pthread_t, ThreadData*>> thread_data;
    return *thread_data;
}

static auto& thread_data_lock()
{
    static NeverDestroyed<Sync::RWLock> lock;
    return *lock;
}

static auto& thread_data_key_once()
{
    static NeverDestroyed<Sync::OnceFlag> once;
    return *once;
}

static void ensure_thread_data_key()
{
    Sync::call_once(thread_data_key_once(), [] {
        VERIFY(pthread_key_create(&s_this_thread_data_key, destroy_thread_data) == 0);
    });
}

struct ThreadData {
    static ThreadData& the()
    {
        ensure_thread_data_key();
        ThreadData* data = nullptr;
        if (!s_this_thread_data) {
            data = new ThreadData;
            s_this_thread_data = data;
            VERIFY(pthread_setspecific(s_this_thread_data_key, s_this_thread_data) == 0);

            Sync::RWLockLocker<Sync::LockMode::Write> locker(thread_data_lock());
            thread_data().set(s_this_thread_data->thread_id, s_this_thread_data);
        } else {
            data = s_this_thread_data;
        }
        return *data;
    }

    static ThreadData* for_thread(pthread_t thread_id)
    {
        // NOTE: thread_data_lock() is supposed to be held by the caller.
        return thread_data().get(thread_id).value_or(nullptr);
    }

    ThreadData()
        : thread_id(pthread_self()), wake_data(make<EventLoopWake>())
    {
        wake_data->type = CompletionType::Wake;
        wake_data->wait_event.handle = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Consider a way for different event loops to have a different number of threads
        iocp.handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
        VERIFY(iocp.handle);

        NTSTATUS status = g_system.NtCreateWaitCompletionPacket(&wake_data->wait_packet.handle, GENERIC_READ | GENERIC_WRITE, NULL);
        VERIFY(NT_SUCCESS(status));
        status = g_system.NtAssociateWaitCompletionPacket(wake_data->wait_packet.handle, iocp.handle, wake_data->wait_event.handle, wake_data.ptr(), NULL, 0, 0, NULL);
        VERIFY(NT_SUCCESS(status));
    }
    ~ThreadData()
    {
        NTSTATUS status = g_system.NtCancelWaitCompletionPacket(wake_data->wait_packet.handle, TRUE);
        VERIFY(NT_SUCCESS(status));
    }

    Sync::RecursiveMutex mutex;

    pthread_t thread_id { };

    OwnHandle iocp;

    // These are only used to register and unregister. The event loop doesn't access these.
    HashMap<intptr_t, NonnullOwnPtr<EventLoopTimer>> timers;
    HashMap<Notifier*, NonnullOwnPtr<EventLoopNotifier>> notifiers;

    // The wake completion packet is posted to the thread's event loop to wake it.
    NonnullOwnPtr<EventLoopWake> wake_data;
};

static void destroy_thread_data(void* value)
{
    s_this_thread_data = nullptr;
    delete static_cast<ThreadData*>(value);
}

static Sync::MutexProtected<HashMap<pid_t, NonnullOwnPtr<EventLoopProcess>>> s_processes;

EventLoopImplementationWindows::EventLoopImplementationWindows()
    : m_wake_event(ThreadData::the().wake_data->wait_event.handle)
{
    VERIFY(m_wake_event);
}

EventLoopImplementationWindows::~EventLoopImplementationWindows()
{
}

int EventLoopImplementationWindows::exec()
{
    for (;;) {
        if (m_exit_requested)
            return m_exit_code;
        pump(PumpMode::WaitForEvents);
    }
    VERIFY_NOT_REACHED();
}

static constexpr bool debug_event_loop = false;

size_t EventLoopImplementationWindows::pump(PumpMode pump_mode)
{
    auto& thread_data = ThreadData::the();
    Sync::MutexLocker locker(thread_data.mutex);
    
    auto& event_queue = ThreadEventQueue::current();

    // NOTE: The number of entries to dequeue is to be optimized. Ideally we always dequeue all outstanding packets,
    // but we don't want to increase the cost of each pump unnecessarily. If more than one entry is never dequeued
    // at once, we could switch to using GetQueuedCompletionStatus which directly returns the values.
    constexpr ULONG entry_count = 32;
    OVERLAPPED_ENTRY entries[entry_count];
    ULONG entries_removed = 0;

    bool has_pending_events = event_queue.has_pending_events();
    DWORD timeout = 0;
    if (!has_pending_events && pump_mode == PumpMode::WaitForEvents)
        timeout = INFINITE;

    BOOL success = GetQueuedCompletionStatusEx(thread_data.iocp.handle, entries, entry_count, &entries_removed, timeout, FALSE);
    dbgln_if(debug_event_loop, "Event loop dequed {} events", entries_removed);

    if (success) {
        for (ULONG i = 0; i < entries_removed; i++) {
            auto& entry = entries[i];
            auto* packet = reinterpret_cast<CompletionPacket*>(entry.lpCompletionKey);

            if (packet->type == CompletionType::Wake) {
                auto* wake_data = static_cast<EventLoopWake*>(packet);
                NTSTATUS status = g_system.NtAssociateWaitCompletionPacket(wake_data->wait_packet.handle, thread_data.iocp.handle, wake_data->wait_event.handle, wake_data, NULL, 0, 0, NULL);
                VERIFY(NT_SUCCESS(status));
                continue;
            }
            if (packet->type == CompletionType::Timer) {
                auto* timer = static_cast<EventLoopTimer*>(packet);
                if (auto owner = timer->owner.strong_ref())
                    event_queue.post_event(owner, Event::Type::Timer);
                if (timer->is_periodic) {
                    NTSTATUS status = g_system.NtAssociateWaitCompletionPacket(timer->wait_packet.handle, thread_data.iocp.handle, timer->timer.handle, timer, NULL, 0, 0, NULL);
                    VERIFY(NT_SUCCESS(status));
                }
                continue;
            }
            if (packet->type == CompletionType::Notifer) {
                auto* notifier_data = static_cast<EventLoopNotifier*>(packet);
                if (auto notifier = notifier_data->notifier.strong_ref())
                    event_queue.post_event(notifier, Core::Event::Type::NotifierActivation);
                NTSTATUS status = g_system.NtAssociateWaitCompletionPacket(notifier_data->wait_packet.handle, thread_data.iocp.handle, notifier_data->wait_event.handle, notifier_data, NULL, 0, 0, NULL);
                VERIFY(NT_SUCCESS(status));
                continue;
            }
            if (packet->type == CompletionType::Process) {
                auto* process_data = static_cast<EventLoopProcess*>(packet);
                pid_t const process_id = process_data->pid;
                // NOTE: This may seem like the incorrect parameter, but https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_associate_completion_port
                // states that this field represents the event type indicator
                DWORD const event_type = entry.dwNumberOfBytesTransferred;
                if (reinterpret_cast<intptr_t>(entry.lpOverlapped) == process_id && (event_type == JOB_OBJECT_MSG_EXIT_PROCESS || event_type == JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS)) {
                    auto const owned_process = s_processes.with_locked([&](auto const& processes) {
                        return processes.get(process_id);
                    });
                    if (owned_process.has_value())
                        owned_process.value()->exit_handler(process_id);
                }
                continue;
            }
            VERIFY_NOT_REACHED();
        }
    } else {
        DWORD error = GetLastError();
        switch (error) {
        case WAIT_TIMEOUT:
            break;
        default:
            dbgln("GetQueuedCompletionStatusEx failed with unexpected error: {}", Error::from_windows_error(error));
            VERIFY_NOT_REACHED();
        }
    }

    return event_queue.process();
}

void EventLoopImplementationWindows::quit(int code)
{
    m_exit_requested = true;
    m_exit_code = code;
}

void EventLoopImplementationWindows::wake()
{
    SetEvent(m_wake_event);
}

static int notifier_type_to_network_event(NotificationType type)
{
    int events = 0;

    if (has_flag(type, NotificationType::Read))
        events |= FD_READ | FD_ACCEPT | FD_CLOSE;

    if (has_flag(type, NotificationType::Write))
        events |= FD_WRITE | FD_CLOSE;

    // Note: NotificationType::Error and NotificationType::HangUp don't map to specific
    // Windows events in the same way as Unix POLLERR/POLLHUP. However, FD_CLOSE can be
    // used to detect socket closure/errors. We include FD_CLOSE when Read/Write are set,
    // and also allow it to be registered standalone for error-only notifiers.
    if (has_flag(type, NotificationType::Error) || has_flag(type, NotificationType::HangUp)) {
        // If no Read/Write flags are set, register FD_CLOSE by itself
        if (events == 0)
            events |= FD_CLOSE;
    }

    if (events == 0) {
        dbgln("No valid notification type specified: {}", (int)type);
        VERIFY_NOT_REACHED();
    }

    return events;
}

void EventLoopManagerWindows::register_notifier(Notifier& notifier)
{
    auto& thread_data = ThreadData::the();
    Sync::MutexLocker locker(thread_data.mutex);
    
    auto& notifiers = thread_data.notifiers;

    if (notifiers.contains(&notifier))
        return;

    HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
    VERIFY(event);
    int rc = WSAEventSelect(notifier.fd(), event, notifier_type_to_network_event(notifier.type()));
    VERIFY(!rc);

    auto notifier_data = make<EventLoopNotifier>();
    notifier_data->type = CompletionType::Notifer;
    notifier_data->notifier = notifier.make_weak_ptr();;
    notifier_data->wait_event.handle = event;
    NTSTATUS status = g_system.NtCreateWaitCompletionPacket(&notifier_data->wait_packet.handle, GENERIC_READ | GENERIC_WRITE, NULL);
    VERIFY(NT_SUCCESS(status));
    status = g_system.NtAssociateWaitCompletionPacket(notifier_data->wait_packet.handle, thread_data.iocp.handle, event, notifier_data.ptr(), NULL, 0, 0, NULL);
    VERIFY(NT_SUCCESS(status));
    notifiers.set(&notifier, move(notifier_data));
}

void EventLoopManagerWindows::unregister_notifier(Notifier& notifier)
{
    Sync::RWLockLocker<Sync::LockMode::Read> locker(thread_data_lock());
    auto* thread_data = ThreadData::for_thread(notifier.owner_thread());
    if (!thread_data)
        return;
    Sync::MutexLocker thread_data_content_locker(thread_data->mutex);
    
    auto& notifiers = thread_data->notifiers;
    auto maybe_notifier_data = notifiers.take(&notifier);
    if (!maybe_notifier_data.has_value())
        return;
    auto notifier_data = move(maybe_notifier_data.value());
    // We are removing the signalled packets since the caller no longer expects them
    NTSTATUS status = g_system.NtCancelWaitCompletionPacket(notifier_data->wait_packet.handle, TRUE);
    VERIFY(NT_SUCCESS(status));
    // TODO: Reuse the data structure
}

intptr_t EventLoopManagerWindows::register_timer(EventReceiver& object, int milliseconds, bool should_reload)
{
    VERIFY(milliseconds >= 0);
    auto& thread_data = ThreadData::the();
    Sync::MutexLocker locker(thread_data.mutex);
    auto& timers = thread_data.timers;

    // FIXME: This is a temporary fix for issue #3641
    bool manual_reset = static_cast<Timer&>(object).is_single_shot();
    HANDLE timer = CreateWaitableTimer(NULL, manual_reset, NULL);
    VERIFY(timer);

    auto timer_data = make<EventLoopTimer>();
    timer_data->type = CompletionType::Timer;
    timer_data->timer.handle = timer;
    timer_data->owner = object.make_weak_ptr();
    timer_data->owner_thread = thread_data.thread_id;
    timer_data->is_periodic = should_reload;
    VERIFY(timer_data->timer.handle);

    NTSTATUS status = g_system.NtCreateWaitCompletionPacket(&timer_data->wait_packet.handle, GENERIC_READ | GENERIC_WRITE, NULL);
    VERIFY(NT_SUCCESS(status));

    LARGE_INTEGER first_time = {};
    // Measured in 0.1μs intervals, negative means starting from now
    first_time.QuadPart = -10'000LL * milliseconds;
    BOOL succeeded = SetWaitableTimer(timer_data->timer.handle, &first_time, should_reload ? milliseconds : 0, NULL, NULL, FALSE);
    VERIFY(succeeded);

    status = g_system.NtAssociateWaitCompletionPacket(timer_data->wait_packet.handle, thread_data.iocp.handle, timer_data->timer.handle, timer_data.ptr(), NULL, 0, 0, NULL);
    VERIFY(NT_SUCCESS(status));

    auto timer_id = reinterpret_cast<intptr_t>(timer_data.ptr());
    VERIFY(!timers.get(timer_id).has_value());
    timers.set(timer_id, move(timer_data));
    return timer_id;
}

void EventLoopManagerWindows::unregister_timer(intptr_t timer_id)
{
    auto* timer_ptr = bit_cast<EventLoopTimer*>(timer_id);
    Sync::RWLockLocker<Sync::LockMode::Read> locker(thread_data_lock());
    auto* thread_data_ptr = ThreadData::for_thread(timer_ptr->owner_thread);
    if (!thread_data_ptr)
        return;
    
    Sync::MutexLocker thread_data_content_locker(thread_data_ptr->mutex);
    auto& thread_data = *thread_data_ptr;
    
    auto maybe_timer = thread_data.timers.take(timer_id);
    if (!maybe_timer.has_value())
        return;
    auto timer = move(maybe_timer.value());
    NTSTATUS status = g_system.NtCancelWaitCompletionPacket(timer->wait_packet.handle, TRUE);
    VERIFY(NT_SUCCESS(status));
}

int EventLoopManagerWindows::register_signal([[maybe_unused]] int signal_number, [[maybe_unused]] Function<void(int)> handler)
{
    dbgln("Core::EventLoopManagerWindows::register_signal() is not implemented");
    VERIFY_NOT_REACHED();
}

void EventLoopManagerWindows::unregister_signal([[maybe_unused]] int handler_id)
{
    dbgln("Core::EventLoopManagerWindows::unregister_signal() is not implemented");
    VERIFY_NOT_REACHED();
}

void EventLoopManagerWindows::register_process(pid_t pid, ESCAPING Function<void(pid_t)> exit_handler)
{
    s_processes.with_locked([&](auto& processes) {
        if (processes.contains(pid))
            return;

        HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        VERIFY(process_handle);

        HANDLE job_object_handle = CreateJobObject(nullptr, nullptr);
        VERIFY(job_object_handle);

        BOOL succeeded = AssignProcessToJobObject(job_object_handle, process_handle);
        VERIFY(succeeded);

        auto process_data = make<EventLoopProcess>();
        process_data->type = CompletionType::Process;
        process_data->process.handle = process_handle;
        process_data->pid = pid;
        process_data->exit_handler = move(exit_handler);
        process_data->jobobject.handle = job_object_handle;

        JOBOBJECT_ASSOCIATE_COMPLETION_PORT joacp = { .CompletionKey = process_data.ptr(), .CompletionPort = ThreadData::the().iocp.handle };
        succeeded = SetInformationJobObject(job_object_handle, JobObjectAssociateCompletionPortInformation, &joacp, sizeof(JOBOBJECT_ASSOCIATE_COMPLETION_PORT));
        VERIFY(succeeded);

        processes.set(pid, move(process_data));
    });
}

void EventLoopManagerWindows::unregister_process(pid_t pid)
{
    auto maybe_process = s_processes.with_locked([&](auto& processes) {
        return processes.take(pid);
    });
    if (!maybe_process.has_value())
        return;

    auto process_data = maybe_process.release_value();
    JOBOBJECT_ASSOCIATE_COMPLETION_PORT joacp = { .CompletionKey = process_data, .CompletionPort = nullptr };
    BOOL succeeded = SetInformationJobObject(process_data->jobobject.handle, JobObjectAssociateCompletionPortInformation, &joacp, sizeof(JOBOBJECT_ASSOCIATE_COMPLETION_PORT));
    VERIFY(succeeded);
}

void EventLoopManagerWindows::did_post_event()
{
}

NonnullOwnPtr<EventLoopImplementation> EventLoopManagerWindows::make_implementation()
{
    return make<EventLoopImplementationWindows>();
}

}
