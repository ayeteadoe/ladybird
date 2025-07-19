/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteString.h>
#include <AK/HashMap.h>
#include <AK/RefPtr.h>
#include <LibCore/FileWatcher.h>

#include <AK/Windows.h>

namespace Core {


struct FileWatcher::WindowsWatch {
    HANDLE dir_handle;
    HANDLE event_handle;
    NonnullRefPtr<Notifier> notifier;
    FileWatcherEvent::Type event_mask;
    ByteString path;
    Array<BYTE, 8192> buffer;
    OVERLAPPED overlapped;

    WindowsWatch(HANDLE dir, HANDLE event, NonnullRefPtr<Notifier> notif, FileWatcherEvent::Type mask, ByteString p)
        : dir_handle(dir)
        , event_handle(event)
        , notifier(move(notif))
        , event_mask(mask)
        , path(move(p))
        , overlapped({})
    {
        overlapped.hEvent = event_handle;
    }

    ~WindowsWatch()
    {
        if (dir_handle != INVALID_HANDLE_VALUE)
            CloseHandle(dir_handle);
        if (event_handle != INVALID_HANDLE_VALUE)
            CloseHandle(event_handle);
    }
};

ErrorOr<bool> FileWatcherBase::add_watch(ByteString path, FileWatcherEvent::Type event_mask)
{
    if (m_path_to_wd.find(path) != m_path_to_wd.end()) {
        dbgln("add_watch: path '{}' is already being watched", path);
        return false;
    }

    // Open the directory for monitoring
    HANDLE dir_handle = CreateFileA(
        path.characters(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);

    if (dir_handle == INVALID_HANDLE_VALUE) {
        return Error::from_windows_error();
    }

    // Create an event handle for this watch
    HANDLE event_handle = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!event_handle) {
        CloseHandle(dir_handle);
        return Error::from_windows_error();
    }

    // Create a notifier for this specific watch
    int event_fd = to_fd(event_handle);
    auto notifier = TRY(Notifier::try_create(event_fd, Notifier::Type::Read));

    // Create the watch object
    auto watch = make<FileWatcher::WindowsWatch>(dir_handle, event_handle, move(notifier), event_mask, path);

    // Set up the notifier callback
    watch->notifier->on_activation = [this, watch_ptr = watch.ptr()] {
        static_cast<FileWatcher*>(this)->process_directory_changes(*watch_ptr);
    };

    // Store the watch
    unsigned wd = to_fd(dir_handle);
    m_path_to_wd.set(path, wd);
    m_wd_to_path.set(wd, path);

    // Start monitoring
    static_cast<FileWatcher*>(this)->start_monitoring(*watch);

    // Store the watch object
    static_cast<FileWatcher*>(this)->m_watches.set(path, move(watch));

    dbgln("add_watch: watching path '{}' with wd {}", path, wd);
    return true;
}

ErrorOr<bool> FileWatcherBase::remove_watch(ByteString path)
{
    auto it = m_path_to_wd.find(path);
    if (it == m_path_to_wd.end()) {
        dbgln("remove_watch: path '{}' is not being watched", path);
        return false;
    }

    unsigned wd = it->value;
    m_path_to_wd.remove(path);
    m_wd_to_path.remove(wd);

    // Remove the watch object (this will clean up handles)
    static_cast<FileWatcher*>(this)->m_watches.remove(path);

    dbgln("remove_watch: stopped watching path '{}'", path);
    return true;
}

FileWatcher::FileWatcher()
    : FileWatcherBase(-1)
{
    //m_notifier->set_enabled(false);
    //
    // m_notifier->on_activation = [this] {
    //     for (auto& entry : m_wd_to_path) {
    //         HANDLE dir_handle = to_handle(entry.key);
    //         BYTE buffer[8192];
    //         DWORD bytes_returned = 0;
    //         OVERLAPPED overlapped = {};
    //         overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    //         if (!overlapped.hEvent)
    //             continue;
    //
    //         // Reissue the monitoring call
    //         DWORD notify_filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION;
    //         BOOL success = ReadDirectoryChangesW(
    //             dir_handle,
    //             buffer,
    //             sizeof(buffer),
    //             TRUE,
    //             notify_filter,
    //             &bytes_returned,
    //             &overlapped,
    //             NULL);
    //         if (!success) {
    //             CloseHandle(overlapped.hEvent);
    //             continue;
    //         }
    //
    //         // Process events
    //         FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
    //         while (true) {
    //             FileWatcherEvent event;
    //             event.event_path = entry.value;
    //
    //             // Map Windows events to FileWatcherEvent::Type
    //             switch (info->Action) {
    //             case FILE_ACTION_ADDED:
    //                 event.type = FileWatcherEvent::Type::ChildCreated;
    //                 break;
    //             case FILE_ACTION_REMOVED:
    //                 event.type = FileWatcherEvent::Type::ChildDeleted;
    //                 break;
    //             case FILE_ACTION_MODIFIED:
    //                 event.type = FileWatcherEvent::Type::ContentModified;
    //                 break;
    //             case FILE_ACTION_RENAMED_OLD_NAME:
    //             case FILE_ACTION_RENAMED_NEW_NAME:
    //                 event.type = FileWatcherEvent::Type::Deleted;
    //                 break;
    //             default:
    //                 continue;
    //             }
    //
    //             if (on_change)
    //                 on_change(event);
    //
    //             if (!info->NextEntryOffset)
    //                 break;
    //             info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
    //                 reinterpret_cast<BYTE*>(info) + info->NextEntryOffset);
    //         }
    //
    //         CloseHandle(overlapped.hEvent);
    //     }
    // };
}

FileWatcher::~FileWatcher()
{
    for (auto& entry : m_wd_to_path) {
        HANDLE dir_handle = to_handle(entry.key);
        CloseHandle(dir_handle);
    }
    if (m_watcher_fd != -1) {
        HANDLE event_handle = to_handle(m_watcher_fd);
        CloseHandle(event_handle);
    }
}

ErrorOr<NonnullRefPtr<FileWatcher>> FileWatcher::create(FileWatcherFlags)
{
    return adopt_ref(*new FileWatcher());
}



DWORD FileWatcher::event_mask_to_notify_filter(FileWatcherEvent::Type event_mask)
{
    DWORD notify_filter = 0;

    if (has_flag(event_mask, FileWatcherEvent::Type::ChildCreated))
        notify_filter |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME;
    if (has_flag(event_mask, FileWatcherEvent::Type::ChildDeleted))
        notify_filter |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME;
    if (has_flag(event_mask, FileWatcherEvent::Type::Deleted))
        notify_filter |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME;
    if (has_flag(event_mask, FileWatcherEvent::Type::ContentModified))
        notify_filter |= FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE;
    if (has_flag(event_mask, FileWatcherEvent::Type::MetadataModified))
        notify_filter |= FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION;

    return notify_filter;
}

void FileWatcher::process_directory_changes(WindowsWatch& watch)
{
    DWORD bytes_returned = 0;
    BOOL success = GetOverlappedResult(watch.dir_handle, &watch.overlapped, &bytes_returned, FALSE);

    if (!success || bytes_returned == 0) {
        // Reissue the monitoring call
        // start_monitoring(watch);
        return;
    }

    // Process the events
    FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(watch.buffer.data());

    while (true) {
        FileWatcherEvent event;

        // Convert the filename from wide string to ByteString
        ByteString filename;
        if (info->FileNameLength > 0) {
            // Convert UTF-16 to UTF-8
            int utf8_length = WideCharToMultiByte(CP_UTF8, 0, info->FileName, info->FileNameLength / sizeof(WCHAR), nullptr, 0, nullptr, nullptr);
            if (utf8_length > 0) {
                Vector<char> utf8_buffer;
                utf8_buffer.resize(utf8_length);
                WideCharToMultiByte(CP_UTF8, 0, info->FileName, info->FileNameLength / sizeof(WCHAR), utf8_buffer.data(), utf8_length, nullptr, nullptr);
                filename = ByteString(utf8_buffer.data(), utf8_length);
            }
        }

        // Build the full path
        if (!filename.is_empty()) {
            event.event_path = ByteString::formatted("{}{}", watch.path, filename);
        } else {
            event.event_path = watch.path;
        }

        // Map Windows events to FileWatcherEvent::Type
        switch (info->Action) {
        case FILE_ACTION_ADDED:
            event.type = FileWatcherEvent::Type::ChildCreated;
            break;
        case FILE_ACTION_REMOVED:
            event.type = FileWatcherEvent::Type::ChildDeleted;
            break;
        case FILE_ACTION_MODIFIED:
            event.type = FileWatcherEvent::Type::ContentModified;
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            event.type = FileWatcherEvent::Type::ChildDeleted;
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            event.type = FileWatcherEvent::Type::ChildCreated;
            break;
        default:
            break;
        }

        // Only fire events that match our mask
        if (has_flag(watch.event_mask, event.type) && on_change) {
            on_change(event);
        }

        if (info->NextEntryOffset == 0)
            break;
        info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
            reinterpret_cast<BYTE*>(info) + info->NextEntryOffset);
    }

    // // Reissue the monitoring call
    // start_monitoring(watch);
}

void FileWatcher::start_monitoring(WindowsWatch& watch)
{
    DWORD notify_filter = event_mask_to_notify_filter(watch.event_mask);
    DWORD bytes_returned = 0;

    // Reset the overlapped structure
    watch.overlapped = {};
    watch.overlapped.hEvent = watch.event_handle;

    BOOL success = ReadDirectoryChangesW(
        watch.dir_handle,
        watch.buffer.data(),
        watch.buffer.size(),
        TRUE, // Watch subtree
        notify_filter,
        &bytes_returned,
        &watch.overlapped,
        NULL);

    if (!success) {
        dbgln("ReadDirectoryChangesW failed for path '{}': {}", watch.path, GetLastError());
    }
}


}
