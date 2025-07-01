/*
 * Copyright (c) 2022, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2023, Matthew Olsson <mattco@serenityos.org>
 * Copyright (c) 2023-2025, Shannon Booth <shannon@serenityos.org>
 * Copyright (c) 2023-2024, Kenneth Myhra <kennethmyhra@serenityos.org>
 * Copyright (c) 2025, Tim Flynn <trflynn89@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGC/Ptr.h>
#include <LibWeb/Forward.h>
#include <LibWeb/Streams/Algorithms.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::Streams {

// 5.5.1. Working with writable streams, https://streams.spec.whatwg.org/#ws-abstract-ops
WEB_API WebIDL::ExceptionOr<GC::Ref<WritableStreamDefaultWriter>> acquire_writable_stream_default_writer(WritableStream&);
WEB_API WebIDL::ExceptionOr<GC::Ref<WritableStream>> create_writable_stream(JS::Realm& realm, GC::Ref<StartAlgorithm> start_algorithm, GC::Ref<WriteAlgorithm> write_algorithm, GC::Ref<CloseAlgorithm> close_algorithm, GC::Ref<AbortAlgorithm> abort_algorithm, double high_water_mark, GC::Ref<SizeAlgorithm> size_algorithm);
WEB_API void initialize_writable_stream(WritableStream&);
WEB_API bool is_writable_stream_locked(WritableStream const&);
WEB_API WebIDL::ExceptionOr<void> set_up_writable_stream_default_writer(WritableStreamDefaultWriter&, WritableStream&);
WEB_API GC::Ref<WebIDL::Promise> writable_stream_abort(WritableStream&, JS::Value reason);
WEB_API GC::Ref<WebIDL::Promise> writable_stream_close(WritableStream&);

// 5.5.2. Interfacing with controllers, https://streams.spec.whatwg.org/#ws-abstract-ops-used-by-controllers
WEB_API GC::Ref<WebIDL::Promise> writable_stream_add_write_request(WritableStream&);
WEB_API bool writable_stream_close_queued_or_in_flight(WritableStream const&);
WEB_API void writable_stream_deal_with_rejection(WritableStream&, JS::Value error);
WEB_API void writable_stream_finish_erroring(WritableStream&);
WEB_API void writable_stream_finish_in_flight_close(WritableStream&);
WEB_API void writable_stream_finish_in_flight_close_with_error(WritableStream&, JS::Value error);
WEB_API void writable_stream_finish_in_flight_write(WritableStream&);
WEB_API void writable_stream_finish_in_flight_write_with_error(WritableStream&, JS::Value error);
WEB_API bool writable_stream_has_operation_marked_in_flight(WritableStream const&);
WEB_API void writable_stream_mark_close_request_in_flight(WritableStream&);
WEB_API void writable_stream_mark_first_write_request_in_flight(WritableStream&);
WEB_API void writable_stream_reject_close_and_closed_promise_if_needed(WritableStream&);
WEB_API void writable_stream_start_erroring(WritableStream&, JS::Value reason);
WEB_API void writable_stream_update_backpressure(WritableStream&, bool backpressure);

// 5.5.3. Writers, https://streams.spec.whatwg.org/#ws-writer-abstract-ops
WEB_API GC::Ref<WebIDL::Promise> writable_stream_default_writer_abort(WritableStreamDefaultWriter&, JS::Value reason);
WEB_API GC::Ref<WebIDL::Promise> writable_stream_default_writer_close(WritableStreamDefaultWriter&);
WEB_API GC::Ref<WebIDL::Promise> writable_stream_default_writer_close_with_error_propagation(WritableStreamDefaultWriter&);
WEB_API void writable_stream_default_writer_ensure_closed_promise_rejected(WritableStreamDefaultWriter&, JS::Value error);
WEB_API void writable_stream_default_writer_ensure_ready_promise_rejected(WritableStreamDefaultWriter&, JS::Value error);
WEB_API Optional<double> writable_stream_default_writer_get_desired_size(WritableStreamDefaultWriter const&);
WEB_API void writable_stream_default_writer_release(WritableStreamDefaultWriter&);
WEB_API GC::Ref<WebIDL::Promise> writable_stream_default_writer_write(WritableStreamDefaultWriter&, JS::Value chunk);

// 5.5.4. Default controllers, https://streams.spec.whatwg.org/#ws-default-controller-abstract-ops
WEB_API WebIDL::ExceptionOr<void> set_up_writable_stream_default_controller(WritableStream&, WritableStreamDefaultController&, GC::Ref<StartAlgorithm>, GC::Ref<WriteAlgorithm>, GC::Ref<CloseAlgorithm>, GC::Ref<AbortAlgorithm>, double high_water_mark, GC::Ref<SizeAlgorithm>);
WEB_API WebIDL::ExceptionOr<void> set_up_writable_stream_default_controller_from_underlying_sink(WritableStream&, JS::Value underlying_sink_value, UnderlyingSink&, double high_water_mark, GC::Ref<SizeAlgorithm> size_algorithm);
WEB_API void writable_stream_default_controller_advance_queue_if_needed(WritableStreamDefaultController&);
WEB_API void writable_stream_default_controller_clear_algorithms(WritableStreamDefaultController&);
WEB_API void writable_stream_default_controller_close(WritableStreamDefaultController&);
WEB_API void writable_stream_default_controller_error(WritableStreamDefaultController&, JS::Value error);
WEB_API void writable_stream_default_controller_error_if_needed(WritableStreamDefaultController&, JS::Value error);
WEB_API bool writable_stream_default_controller_get_backpressure(WritableStreamDefaultController const&);
WEB_API JS::Value writable_stream_default_controller_get_chunk_size(WritableStreamDefaultController&, JS::Value chunk);
WEB_API double writable_stream_default_controller_get_desired_size(WritableStreamDefaultController const&);
WEB_API void writable_stream_default_controller_process_close(WritableStreamDefaultController&);
WEB_API void writable_stream_default_controller_process_write(WritableStreamDefaultController&, JS::Value chunk);
WEB_API void writable_stream_default_controller_write(WritableStreamDefaultController&, JS::Value chunk, JS::Value chunk_size);

}
