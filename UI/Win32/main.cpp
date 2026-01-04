/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/EventLoop.h>
#include <LibCore/Socket.h>
#include <LibCore/System.h>
#include <LibCore/Timer.h>
#include <LibMain/Main.h>
#include <UI/Win32/EventLoopImplementationWin32.h>

ErrorOr<int> ladybird_main([[maybe_unused]] Main::Arguments arguments)
{
    Core::EventLoopManager::install(*new Ladybird::EventLoopManagerWin32);
    Core::EventLoop event_loop;

    int socket_fds[2] {};
    TRY(Core::System::socketpair(AF_LOCAL, SOCK_STREAM, 0, socket_fds));

    auto socket_read = TRY(Core::LocalSocket::adopt_fd(socket_fds[0]));
    socket_read->on_ready_to_read = [&socket_read]() {
        static constexpr size_t buffer_size = 256;
        static char buffer[buffer_size];
        auto bytes = socket_read->read_some({ buffer, buffer_size });
        if (!bytes.is_error()) {
            dbgln("{}", ByteString(reinterpret_cast<char const*>(bytes.value().data()), bytes.value().size()));
        }
    };

    auto timer = Core::Timer::create_repeating(1000, [&socket_fds] {
        static int timer_count = 0;
        auto message = ByteString::formatted("Timer tick {}", ++timer_count);
        [[maybe_unused]] auto result = Core::System::send(socket_fds[1], message.bytes(), 0);
    });
    timer->start();

    return event_loop.exec();
}
