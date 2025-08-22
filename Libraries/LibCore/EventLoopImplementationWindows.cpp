/*
 * Copyright (c) 2023, Andreas Kling <andreas@ladybird.org>
 * Copyright (c) 2024-2025, stasoid <stasoid@yahoo.com>
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/HashMap.h>
#include <LibCore/EventLoopImplementationWindows.h>
#include <LibCore/Notifier.h>
#include <LibCore/ThreadEventQueue.h>
#include <LibCore/Timer.h>

#include <windows.h>
#include <Winternl.h>
#include <ntstatus.h>

#pragma comment(lib, "ntdll.lib")

// NOTE: Including after the above as we undefine IN here but Winternl.h uses that
#include <AK/Windows.h>

// UnlimitedWait approach from https://github.com/tringi/win32-iocp-events, trimmed down the function calls actually used in EventLoopImplementationWindows. NOTE: As of this commit, the repository/source files do not have a license

typedef VOID (WINAPI * PUNLIMITED_WAIT_CALLBACK) (PVOID lpWaitContext);
typedef BOOL (WINAPI * PUNLIMITED_WAIT_OBJECT_CALLBACK) (PVOID lpObjectContext, HANDLE hObject);

struct UnlimitedWait;

// CreateUnlimitedWait
//  - creates new 'UnlimitedWait' object and preallocates some object slots
//  - parameters:
//     - lpWaitContext - user-defined value, that is passed to callback functions
//     - nPreAllocatedSlots - number of slots for object handles to prepare in advance
//     - pfnTimeoutCallback - called with 'lpWaitContext' by WaitUnlimitedWait(Ex) on timeout
//     - pfnApcWakeCallback - called with 'lpWaitContext' by WaitUnlimitedWait(Ex) after APC interrupted the wait
//  - returns:
//     - 'handle' to the UnlimitedWait object to be used in the remaining functions
//     - NULL on error - call 'GetLastError()' to get the underlying reason which can also be HRESULT
//
_Success_ (return != NULL)
UnlimitedWait * WINAPI CreateUnlimitedWait (
    _In_opt_ PVOID lpWaitContext,
    _In_     DWORD nPreAllocatedSlots,
    _In_opt_ PUNLIMITED_WAIT_CALLBACK pfnTimeoutCallback,
    _In_opt_ PUNLIMITED_WAIT_CALLBACK pfnApcWakeCallback
);

// DeleteUnlimitedWait
//  - destroys the object and releases all resources
//  - there is no need to remove individual waited-on object handles
//  - returns: TRUE - on successful cleanup
//             FALSE - when any subcomponent failed to cleanup and memory/handles could've leaked
//                   - note that object that failed to be fully deleted can no longer be used
//
_Success_ (return != FALSE)
BOOL WINAPI DeleteUnlimitedWait (
    _In_ UnlimitedWait * hUnlimitedWait
);

// AddUnlimitedWaitObject
//  - adds object handle to 'UnlimitedWait' and starts consuming signalled state changes
//  - parameters:
//     - 'hObjectHandle' - handle to supported kernel objects (event, semaphore, process or thread)
//     - 'ptrCallbackFunction' - optional function called by WaitUnlimitedWait(Ex)
//                               when the associated object signalled status is retrieved
//     - 'lpObjectContext' - pointer, that is passed to 'ptrCallbackFunction' (if provided) and
//                           returned by WaitUnlimitedWait(Ex) to identify signalled objects
//  - returns: TRUE - on success
//             FALSE - on failure, call GetLastError () to get more information:
//                   - may contain HRESULT on unexpected failures
//
_Success_ (return != FALSE)
BOOL WINAPI AddUnlimitedWaitObject (
    _In_     UnlimitedWait * hUnlimitedWait,
    _In_     HANDLE          hObjectHandle,
    _In_opt_ PUNLIMITED_WAIT_OBJECT_CALLBACK ptrCallbackFunction,
    _In_opt_ PVOID           lpObjectContext
);

// RemoveUnlimitedWaitObject
//  - removes object from 'UnlimitedWait' and stops consuming signalled state changes
//  - parameters:
//     - 'hObjectHandle' - handle to kernel object already added
//     - 'bKeepSignalsEnqueued' - TRUE - WaitUnlimitedWait(Ex) will still retrieve remaining signals that
//                                       occured before call to 'RemoveUnlimitedWaitObject'
//                              - FALSE - all unretrieved signals that occured before this call are deleted
//  - returns: TRUE - on successful removal
//             FALSE - on failure, call GetLastError () to get more information:
//                   - ERROR_FILE_NOT_FOUND - the 'hObjectHandle' is not associated with this UnlimitedWait,
//                                            possibly removed by callback function returning FALSE
//                   - may contain HRESULT on unexpected failures
//
_Success_ (return != FALSE)
BOOL WINAPI RemoveUnlimitedWaitObject (
    _In_ UnlimitedWait * hUnlimitedWait,
    _In_ HANDLE          hObjectHandle,
    _In_ BOOL            bKeepSignalsEnqueued
);

// WaitUnlimitedWait
//  - retrieves one (the oldest) object signalled status notifications
//  - calls 'ptrCallbackFunction' for that signalled object, if set
//  - parameters:
//     - lpSignalledObjectContext - receives context for the signalled object
//     - dwMilliseconds - when should the function fail (with WAIT_TIMEOUT error) if there's no notification
//                      - the 'pfnTimeoutCallback' function is called when that happens
//     - bAlertable - whether the function should process User APCs (and then fail with WAIT_IO_COMPLETION error)
//                  - the 'pfnApcWakeCallback' function is called when that happens
//  - return value models standard Windows wait APIs:
//     - TRUE if a notification was retrieved
//     - FALSE if no object was signalled, call GetLastError () to get more information:
//        - successes
//           - WAIT_IO_COMPLETION - User APC was retrieved and processed, due to 'bAlertable' being TRUE
//           - WAIT_TIMEOUT - 'dwMilliseconds' elapsed without any completion or User APC
//        - all other errors are hard values and :
//           - ERROR_ABANDONED_WAIT_0 - UnlimitedWait was deleted from underneath the function
//
_Success_ (return != FALSE)
BOOL WINAPI WaitUnlimitedWait (
    _In_      UnlimitedWait * hUnlimitedWait,
    _Out_opt_ PVOID *         lpSignalledObjectContext,
    _In_      DWORD           dwMilliseconds,
    _In_      BOOL            bAlertable
);

struct Handle {
    HANDLE handle = NULL;

    explicit Handle(HANDLE h = NULL)
        : handle(h)
    {
    }
    Handle(Handle&& h)
    {
        handle = h.handle;
        h.handle = NULL;
    }
    void operator=(Handle&& h)
    {
        VERIFY(!handle);
        handle = h.handle;
        h.handle = NULL;
    }
    ~Handle()
    {
        if (handle)
            CloseHandle(handle);
    }

    bool operator==(Handle const& h) const { return handle == h.handle; }
    bool operator==(HANDLE h) const { return handle == h; }
};

template<>
struct Traits<Handle> : DefaultTraits<Handle> {
    static unsigned hash(Handle const& h) { return Traits<HANDLE>::hash(h.handle); }
};
template<>
constexpr bool IsHashCompatible<HANDLE, Handle> = true;

namespace Core {

struct EventLoopTimer {
    WeakPtr<EventReceiver> owner;
};

class WaitData {
public:
    ~WaitData() = default;
    virtual void post_event(ThreadEventQueue& event_queue) = 0;
};

class NotifierWaitData final : public WaitData {
public:
    NotifierWaitData(Notifier& notifier)
        : m_notifier(&notifier)
    {
    }

    void post_event(ThreadEventQueue& event_queue) override
    {
        event_queue.post_event(*m_notifier, make<NotifierActivationEvent>(m_notifier->fd(), m_notifier->type()));
    }

    bool operator==(Notifier* notifier) const { return notifier == m_notifier; }

private:
    Notifier* m_notifier { nullptr };
};

class TimerWaitData final : public WaitData {
public:
    TimerWaitData(EventReceiver& timer)
        : m_timer(timer)
    {
    }

    void post_event(ThreadEventQueue& event_queue) override
    {
        if (auto strong_timer = m_timer.strong_ref())
            event_queue.post_event(*strong_timer, make<TimerEvent>());
    }

private:
    WeakPtr<EventReceiver> m_timer;
};

struct ThreadData {
    static ThreadData* the()
    {
        thread_local OwnPtr<ThreadData> thread_data = make<ThreadData>();
        if (thread_data)
            return &*thread_data;
        return nullptr;
    }

    ThreadData()
    {
        wake_event.handle = CreateEvent(NULL, FALSE, FALSE, NULL);
        VERIFY(wake_event.handle);

        wait = CreateUnlimitedWait(NULL, 0, nullptr, nullptr);
        VERIFY(wait);
    }

    // Each thread has its own timers, notifiers and a wake event.
    HashMap<Handle, NonnullOwnPtr<TimerWaitData>> timers;
    HashMap<Handle, NonnullOwnPtr<NotifierWaitData>> notifiers;

    // The wake event is used to notify another event loop that someone has called wake().
    Handle wake_event;

    UnlimitedWait * volatile wait = nullptr;
};

EventLoopImplementationWindows::EventLoopImplementationWindows()
    : m_wake_event(ThreadData::the()->wake_event.handle)
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

size_t EventLoopImplementationWindows::pump(PumpMode)
{
    auto& event_queue = ThreadEventQueue::current();
    auto* thread_data = ThreadData::the();

    bool has_pending_events = event_queue.has_pending_events();
    int timeout = has_pending_events ? 0 : INFINITE;

    LPVOID object_context = nullptr;
    if (WaitUnlimitedWait (thread_data->wait, &object_context, timeout, TRUE)) {
        auto wait_data = static_cast<WaitData*>(object_context);
        wait_data->post_event(event_queue);
    }
    /*

    auto* thread_data = ThreadData::the();
    auto& notifiers = thread_data->notifiers;
    auto& timers = thread_data->timers;

    size_t event_count = 1 + notifiers.size() + timers.size();
    // If 64 events limit proves to be insufficient RegisterWaitForSingleObject or other methods
    // can be used instead as mentioned in https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitformultipleobjects
    // TODO: investigate if event_count can realistically exceed 64
    VERIFY(event_count <= MAXIMUM_WAIT_OBJECTS);

    Vector<HANDLE, MAXIMUM_WAIT_OBJECTS> event_handles;
    event_handles.append(thread_data->wake_event.handle);

    for (auto& entry : notifiers)
        event_handles.append(entry.key.handle);
    for (auto& entry : timers)
        event_handles.append(entry.key.handle);

    bool has_pending_events = event_queue.has_pending_events();
    int timeout = has_pending_events ? 0 : INFINITE;
    DWORD result = WaitForMultipleObjects(event_count, event_handles.data(), FALSE, timeout);
    if (result == WAIT_TIMEOUT) {
        // FIXME: This verification sometimes fails with ERROR_INVALID_HANDLE, but when I check
        //        the handles they all seem to be valid.
        // VERIFY(GetLastError() == ERROR_SUCCESS || GetLastError() == ERROR_IO_PENDING);
    } else {
        size_t const index = result - WAIT_OBJECT_0;
        VERIFY(index < event_count);
        // : 1 - skip wake event
        for (size_t i = index ? index : 1; i < event_count; i++) {
            // i == index already checked by WaitForMultipleObjects
            if (i == index || WaitForSingleObject(event_handles[i], 0) == WAIT_OBJECT_0) {
                if (i <= notifiers.size()) {
                    Notifier* notifier = *notifiers.get(event_handles[i]);
                    event_queue.post_event(*notifier, make<NotifierActivationEvent>(notifier->fd(), notifier->type()));
                } else {
                    auto& timer = *timers.get(event_handles[i]);
                    if (auto strong_owner = timer.owner.strong_ref())
                        event_queue.post_event(*strong_owner, make<TimerEvent>());
                }
            }
        }
    }
    */

    return event_queue.process();
}

void EventLoopImplementationWindows::quit(int code)
{
    m_exit_requested = true;
    m_exit_code = code;
}

void EventLoopImplementationWindows::post_event(EventReceiver& receiver, NonnullOwnPtr<Event>&& event)
{
    m_thread_event_queue.post_event(receiver, move(event));
    if (&m_thread_event_queue != &ThreadEventQueue::current())
        wake();
}

void EventLoopImplementationWindows::wake()
{
    SetEvent(m_wake_event);
}

static int notifier_type_to_network_event(NotificationType type)
{
    switch (type) {
    case NotificationType::Read:
        return FD_READ | FD_CLOSE | FD_ACCEPT;
    case NotificationType::Write:
        return FD_WRITE;
    default:
        dbgln("This notification type is not implemented: {}", (int)type);
        VERIFY_NOT_REACHED();
    }
}

void EventLoopManagerWindows::register_notifier(Notifier& notifier)
{
    HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
    VERIFY(event);
    int rc = WSAEventSelect(notifier.fd(), event, notifier_type_to_network_event(notifier.type()));
    VERIFY(!rc);

    auto wait_data = make<NotifierWaitData>(notifier);

    BOOL result = AddUnlimitedWaitObject (ThreadData::the()->wait, event, nullptr, wait_data.ptr());
    VERIFY(result);

    auto& notifiers = ThreadData::the()->notifiers;
    VERIFY(!notifiers.get(event).has_value());
    notifiers.set(Handle(event), move(wait_data));
}

void EventLoopManagerWindows::unregister_notifier(Notifier& notifier)
{
    // remove_first_matching would be clearer, but currently there is no such method in HashMap
    if (ThreadData::the())
        ThreadData::the()->notifiers.remove_all_matching([&](auto& key, auto& value) {
            const bool matches = *value == &notifier;
            if (matches) {
                RemoveUnlimitedWaitObject(ThreadData::the()->wait, key.handle, FALSE);
            }
            return matches;
        });
}

intptr_t EventLoopManagerWindows::register_timer(EventReceiver& object, int milliseconds, bool should_reload)
{
    VERIFY(milliseconds >= 0);
    // FIXME: This is a temporary fix for issue #3641
    bool manual_reset = static_cast<Timer&>(object).is_single_shot();
    HANDLE timer = CreateWaitableTimer(NULL, manual_reset, NULL);
    VERIFY(timer);

    LARGE_INTEGER first_time = {};
    // Measured in 0.1μs intervals, negative means starting from now
    first_time.QuadPart = -10'000 * milliseconds;
    BOOL rc = SetWaitableTimer(timer, &first_time, should_reload ? milliseconds : 0, NULL, NULL, FALSE);
    VERIFY(rc);

    auto wait_data = make<TimerWaitData>(object);

    BOOL result = AddUnlimitedWaitObject (ThreadData::the()->wait, timer, nullptr, wait_data.ptr());
    VERIFY(result);

    auto& timers = ThreadData::the()->timers;
    VERIFY(!timers.get(timer).has_value());
    timers.set(Handle(timer), move(wait_data));
    return reinterpret_cast<intptr_t>(timer);
}

void EventLoopManagerWindows::unregister_timer(intptr_t timer_id)
{
    if (ThreadData::the())
        ThreadData::the()->timers.remove_all_matching([&](auto& key, auto&) {
            const bool matches = key.handle == reinterpret_cast<HANDLE>(timer_id);
            if (matches) {
                RemoveUnlimitedWaitObject(ThreadData::the()->wait, key.handle, FALSE);
            }
            return matches;
        });
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

void EventLoopManagerWindows::did_post_event()
{
}

NonnullOwnPtr<EventLoopImplementation> EventLoopManagerWindows::make_implementation()
{
    return make<EventLoopImplementationWindows>();
}

}


extern "C" {
    WINBASEAPI NTSTATUS WINAPI NtCreateWaitCompletionPacket (
        _Out_ PHANDLE WaitCompletionPacketHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
    );
    WINBASEAPI NTSTATUS WINAPI NtAssociateWaitCompletionPacket (
        _In_ HANDLE WaitCompletionPacketHandle,
        _In_ HANDLE IoCompletionHandle,
        _In_ HANDLE TargetObjectHandle,
        _In_opt_ PVOID KeyContext,
        _In_opt_ PVOID ApcContext,
        _In_ NTSTATUS IoStatus,
        _In_ ULONG_PTR IoStatusInformation,
        _Out_opt_ PBOOLEAN AlreadySignaled
    );
    WINBASEAPI NTSTATUS WINAPI NtCancelWaitCompletionPacket (
        _In_ HANDLE WaitCompletionPacketHandle,
        _In_ BOOLEAN RemoveSignaledPacket
    );

    DWORD ConvertHResult (HRESULT hr, HANDLE h) {
        switch (hr) {
            case STATUS_NO_MEMORY:
                return ERROR_OUTOFMEMORY;

            case STATUS_INVALID_HANDLE: // not valid handle passed for hIOCP
            case STATUS_OBJECT_TYPE_MISMATCH: // incorrect handle passed for hIOCP
            case STATUS_INVALID_PARAMETER_1:
            case STATUS_INVALID_PARAMETER_2:
                return ERROR_INVALID_PARAMETER;

            case STATUS_INVALID_PARAMETER_3:
                if (h) {
                    return ERROR_INVALID_HANDLE;
                } else {
                    return ERROR_INVALID_PARAMETER;
                }
        }
        return hr;
    }

    struct UnlimitedWaitSlot {
        HANDLE hWaitPacket;
        HANDLE hObject;
    };
}

struct UnlimitedWait {
    HANDLE  hIOCP;
    SRWLOCK srwLock;
    PVOID   lpWaitContext;
    PUNLIMITED_WAIT_CALLBACK pfnTimeoutCallback;
    PUNLIMITED_WAIT_CALLBACK pfnApcWakeCallback;
    UnlimitedWaitSlot *      slots;
};

_Success_ (return != NULL)
UnlimitedWait * WINAPI CreateUnlimitedWait (
    _In_opt_ PVOID lpWaitContext,
    _In_     DWORD nPreAllocatedSlots,
    _In_opt_ PUNLIMITED_WAIT_CALLBACK pfnTimeoutCallback,
    _In_opt_ PUNLIMITED_WAIT_CALLBACK pfnApcWakeCallback
) {
    HANDLE hHeap = GetProcessHeap ();
    UnlimitedWait * instance = (UnlimitedWait *) HeapAlloc (hHeap, 0, sizeof (UnlimitedWait));

    if (instance) {
        instance->hIOCP = CreateIoCompletionPort (INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (instance->hIOCP) {
            instance->srwLock = SRWLOCK_INIT;
            instance->lpWaitContext = lpWaitContext;
            instance->pfnTimeoutCallback = pfnTimeoutCallback;
            instance->pfnApcWakeCallback = pfnApcWakeCallback;

            if (nPreAllocatedSlots == 0) {
                nPreAllocatedSlots = 8;
            }

            instance->slots = (UnlimitedWaitSlot *) HeapAlloc (hHeap, 0, nPreAllocatedSlots * sizeof (UnlimitedWaitSlot));
            if (instance->slots) {

                HRESULT hr = 0;
                DWORD nCreatedPackets = 0;
                while (SUCCEEDED (hr = NtCreateWaitCompletionPacket (&instance->slots [nCreatedPackets].hWaitPacket, GENERIC_ALL, NULL))) {
                    instance->slots [nCreatedPackets].hObject = NULL;

                    if (++nCreatedPackets == nPreAllocatedSlots) {
                        return instance;
                    }
                }

                while (nCreatedPackets--) {
                    CloseHandle (instance->slots [nCreatedPackets].hWaitPacket);
                }
                HeapFree (hHeap, 0, instance->slots);
                SetLastError (ConvertHResult (hr, NULL));
            } else {
                SetLastError (ERROR_OUTOFMEMORY);
            }

            CloseHandle (instance->hIOCP);
        }
        HeapFree (hHeap, 0, instance);
    } else {
        SetLastError (ERROR_OUTOFMEMORY);
    }
    return NULL;
}

_Success_ (return != FALSE)
BOOL WINAPI DeleteUnlimitedWait (
    _In_ UnlimitedWait * instance
) {
    if (!instance) {
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }

    BOOL result = TRUE;
    HANDLE hHeap = GetProcessHeap ();

    AcquireSRWLockExclusive (&instance->srwLock);

    if (instance->slots) {
        SIZE_T nSlots = HeapSize (hHeap, 0, instance->slots) / sizeof (UnlimitedWaitSlot);
        while (nSlots--) {
            CloseHandle (instance->slots [nSlots].hWaitPacket);
        }

        if (!HeapFree (hHeap, 0, instance->slots)) {
            result = FALSE;
        }
    }

    ReleaseSRWLockExclusive (&instance->srwLock);

    if (!CloseHandle (instance->hIOCP)) {
        result = FALSE;
    }
    if (!HeapFree (hHeap, 0, instance)) {
        result = FALSE;
    }
    return result;
}

namespace {
    BOOL SetAssociation (UnlimitedWait * instance, SIZE_T i, HANDLE hObjectHandle, PVOID ptrCallbackFunction, PVOID lpObjectContext) {

        HRESULT hr = NtAssociateWaitCompletionPacket (instance->slots [i].hWaitPacket, instance->hIOCP, hObjectHandle,
                                                      ptrCallbackFunction, lpObjectContext, 0, i, NULL);
        if (SUCCEEDED (hr)) {
            instance->slots [i].hObject = hObjectHandle;
            return TRUE;

        } else {
            SetLastError (ConvertHResult (hr, hObjectHandle));
            return FALSE;
        }
    }
}

_Success_ (return != FALSE)
BOOL WINAPI AddUnlimitedWaitObject (
    _In_ UnlimitedWait * instance,
    _In_ HANDLE hObjectHandle,
    _In_opt_ PUNLIMITED_WAIT_OBJECT_CALLBACK ptrCallbackFunction,
    _In_opt_ PVOID lpObjectContext
) {
    if (!instance) {
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!hObjectHandle) {
        SetLastError (ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    AcquireSRWLockExclusive (&instance->srwLock);

    HANDLE hHeap = GetProcessHeap ();
    SIZE_T nSlots = HeapSize (hHeap, 0, instance->slots) / sizeof (UnlimitedWaitSlot);

    for (SIZE_T i = 0; i != nSlots; ++i) {
        if ((instance->slots [i].hWaitPacket != NULL) && (instance->slots [i].hObject == NULL)) {
            BOOL result = SetAssociation (instance, i, hObjectHandle, (PVOID) ptrCallbackFunction, (PVOID) lpObjectContext);

            ReleaseSRWLockExclusive (&instance->srwLock);
            return result;
        }
    }

    if (auto newSlots = HeapReAlloc (hHeap, 0, instance->slots, (nSlots + 1) * sizeof (UnlimitedWaitSlot))) {
        instance->slots = (UnlimitedWaitSlot *) newSlots;
        instance->slots [nSlots].hWaitPacket = NULL;
        instance->slots [nSlots].hObject = NULL;

        HRESULT hr = NtCreateWaitCompletionPacket (&instance->slots [nSlots].hWaitPacket, GENERIC_ALL, NULL);
        if (SUCCEEDED (hr)) {

            if (SetAssociation (instance, nSlots, hObjectHandle, (PVOID) ptrCallbackFunction, (PVOID) lpObjectContext)) {
                ReleaseSRWLockExclusive (&instance->srwLock);
                return TRUE;
            }

            NtCancelWaitCompletionPacket (instance->slots [nSlots].hWaitPacket, TRUE);

        } else {
            if (auto revertedSlots = HeapReAlloc (hHeap, 0, instance->slots, nSlots * sizeof (UnlimitedWaitSlot))) {
                instance->slots = (UnlimitedWaitSlot *) revertedSlots;
            } else {
                SetLastError (ERROR_OUTOFMEMORY);
            }
        }
    } else {
        SetLastError (ERROR_OUTOFMEMORY);
    }

    ReleaseSRWLockExclusive (&instance->srwLock);
    return FALSE;
}

_Success_ (return != FALSE)
BOOL WINAPI RemoveUnlimitedWaitObject (
    _In_ UnlimitedWait * instance,
    _In_ HANDLE hObjectHandle,
    _In_ BOOL bKeepSignalsEnqueued
) {
    if (!instance) {
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!hObjectHandle) {
        SetLastError (ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    AcquireSRWLockExclusive (&instance->srwLock);

    SIZE_T nSlots = HeapSize (GetProcessHeap (), 0, instance->slots) / sizeof (UnlimitedWaitSlot);
    for (SIZE_T i = 0; i != nSlots; ++i) {

        if (instance->slots [i].hObject == hObjectHandle) {
            HRESULT hr = NtCancelWaitCompletionPacket (instance->slots [i].hWaitPacket, !bKeepSignalsEnqueued);
            BOOL result = SUCCEEDED (hr);

            if (result) {
                instance->slots [i].hObject = NULL;
            }

            ReleaseSRWLockExclusive (&instance->srwLock);

            if (!result) {
                SetLastError (ConvertHResult (hr, NULL));
            }
            return result;
        }
    }

    ReleaseSRWLockExclusive (&instance->srwLock);
    SetLastError (ERROR_FILE_NOT_FOUND);
    return FALSE;
}

static
BOOL WINAPI WaitUnlimitedWaitExImplementation (
    _In_ UnlimitedWait * instance,
    _Out_writes_to_opt_ (ulCount, *ulNumEntriesProcessed) PVOID * lpSignalledObjectContexts,
    _In_ ULONG ulCount,
    _Out_opt_ ULONG * ulNumEntriesProcessed,
    _Out_writes_all_ (ulCount) OVERLAPPED_ENTRY * oResults,
    _In_ DWORD dwMilliseconds,
    _In_ BOOL bAlertable
) {
    if (!instance) {
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }

    AcquireSRWLockShared (&instance->srwLock);

    DWORD nCompletions;
    if (GetQueuedCompletionStatusEx (instance->hIOCP, oResults, ulCount, &nCompletions, dwMilliseconds, bAlertable)) {

        if (ulNumEntriesProcessed) {
            *ulNumEntriesProcessed = nCompletions;
        }

        BOOL result = TRUE;
        for (ULONG i = 0; i != nCompletions; ++i) {

            BOOL bReRegister;
            if (oResults [i].lpCompletionKey) {

                // TODO: user may want to call add/remove inside the callback

                bReRegister = ((PUNLIMITED_WAIT_OBJECT_CALLBACK) oResults [i].lpCompletionKey) (oResults [i].lpOverlapped,
                                                                                                instance->slots [oResults [i].dwNumberOfBytesTransferred].hObject);
            } else {
                bReRegister = TRUE;
            }

            if (bReRegister) {
                if (!SetAssociation (instance, oResults [i].dwNumberOfBytesTransferred,
                                     instance->slots [oResults [i].dwNumberOfBytesTransferred].hObject,
                                     (PVOID) oResults [i].lpCompletionKey, (PVOID) oResults [i].lpOverlapped)) {

                    result = FALSE;
                }
            } else {
                instance->slots [oResults [i].dwNumberOfBytesTransferred].hObject = NULL;
            }

            if (lpSignalledObjectContexts) {
                lpSignalledObjectContexts [i] = (PVOID) oResults [i].lpOverlapped;
            }
        }

        ReleaseSRWLockShared (&instance->srwLock);
        return result;

    } else {
        if (ulNumEntriesProcessed) {
            *ulNumEntriesProcessed = 0;
        }

        DWORD error = GetLastError ();
        switch (error) {
            case WAIT_TIMEOUT:
                if (instance->pfnTimeoutCallback) {
                    instance->pfnTimeoutCallback (instance->lpWaitContext);
                }
                break;
            case WAIT_IO_COMPLETION:
                if (instance->pfnApcWakeCallback) {
                    instance->pfnApcWakeCallback (instance->lpWaitContext);
                }
                break;

            case ERROR_ABANDONED_WAIT_0:
                // object deleted, srwLock is no longer valid, cannot unlock
                return FALSE;

            default:
                ReleaseSRWLockShared (&instance->srwLock);
                return FALSE;
        }
        ReleaseSRWLockShared (&instance->srwLock);
        SetLastError (error);
        return FALSE;
    }
}

_Success_ (return != FALSE)
BOOL WINAPI WaitUnlimitedWait (
    _In_ UnlimitedWait * instance,
    _Out_opt_ PVOID * lpSignalledObjectContext,
    _In_ DWORD dwMilliseconds,
    _In_ BOOL bAlertable
) {
    OVERLAPPED_ENTRY oResult = {};
    return WaitUnlimitedWaitExImplementation (instance, lpSignalledObjectContext, 1, NULL, &oResult, dwMilliseconds, bAlertable);
}
