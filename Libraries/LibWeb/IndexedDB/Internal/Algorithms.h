/*
 * Copyright (c) 2024, stelar7 <dudedbz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Variant.h>
#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Forward.h>
#include <LibWeb/HTML/DOMStringList.h>
#include <LibWeb/IndexedDB/IDBKeyRange.h>
#include <LibWeb/IndexedDB/IDBRequest.h>
#include <LibWeb/IndexedDB/Internal/Key.h>
#include <LibWeb/StorageAPI/StorageKey.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::IndexedDB {

using KeyPath = Variant<String, Vector<String>>;
using RecordSource = Variant<GC::Ref<ObjectStore>, GC::Ref<Index>>;

WEB_API WebIDL::ExceptionOr<GC::Ref<IDBDatabase>> open_a_database_connection(JS::Realm&, StorageAPI::StorageKey, String, Optional<u64>, GC::Ref<IDBRequest>);
WEB_API bool fire_a_version_change_event(JS::Realm&, FlyString const&, GC::Ref<DOM::EventTarget>, u64, Optional<u64>);
WEB_API WebIDL::ExceptionOr<GC::Ref<Key>> convert_a_value_to_a_key(JS::Realm&, JS::Value, Vector<JS::Value> = {});
WEB_API void close_a_database_connection(GC::Ref<IDBDatabase>, bool forced = false);
WEB_API GC::Ref<IDBTransaction> upgrade_a_database(JS::Realm&, GC::Ref<IDBDatabase>, u64, GC::Ref<IDBRequest>);
WEB_API WebIDL::ExceptionOr<u64> delete_a_database(JS::Realm&, StorageAPI::StorageKey, String, GC::Ref<IDBRequest>);
WEB_API void abort_a_transaction(GC::Ref<IDBTransaction>, GC::Ptr<WebIDL::DOMException>);
WEB_API JS::Value convert_a_key_to_a_value(JS::Realm&, GC::Ref<Key>);
WEB_API bool is_valid_key_path(KeyPath const&);
WEB_API GC::Ref<HTML::DOMStringList> create_a_sorted_name_list(JS::Realm&, Vector<String>);
WEB_API void commit_a_transaction(JS::Realm&, GC::Ref<IDBTransaction>);
WEB_API WebIDL::ExceptionOr<JS::Value> clone_in_realm(JS::Realm&, JS::Value, GC::Ref<IDBTransaction>);
WEB_API WebIDL::ExceptionOr<GC::Ref<Key>> convert_a_value_to_a_multi_entry_key(JS::Realm&, JS::Value);
WEB_API WebIDL::ExceptionOr<ErrorOr<JS::Value>> evaluate_key_path_on_a_value(JS::Realm&, JS::Value, KeyPath const&);
WEB_API WebIDL::ExceptionOr<ErrorOr<GC::Ref<Key>>> extract_a_key_from_a_value_using_a_key_path(JS::Realm&, JS::Value, KeyPath const&, bool = false);
WEB_API bool check_that_a_key_could_be_injected_into_a_value(JS::Realm&, JS::Value, KeyPath const&);
WEB_API void fire_an_error_event(JS::Realm&, GC::Ref<IDBRequest>);
WEB_API void fire_a_success_event(JS::Realm&, GC::Ref<IDBRequest>);
WEB_API GC::Ref<IDBRequest> asynchronously_execute_a_request(JS::Realm&, IDBRequestSource, GC::Ref<GC::Function<WebIDL::ExceptionOr<JS::Value>()>>, GC::Ptr<IDBRequest> = nullptr);
WEB_API ErrorOr<u64> generate_a_key(GC::Ref<ObjectStore>);
WEB_API void possibly_update_the_key_generator(GC::Ref<ObjectStore>, GC::Ref<Key>);
WEB_API void inject_a_key_into_a_value_using_a_key_path(JS::Realm&, JS::Value, GC::Ref<Key>, KeyPath const&);
WEB_API JS::Value delete_records_from_an_object_store(GC::Ref<ObjectStore>, GC::Ref<IDBKeyRange>);
WEB_API WebIDL::ExceptionOr<GC::Ptr<Key>> store_a_record_into_an_object_store(JS::Realm&, GC::Ref<ObjectStore>, JS::Value, GC::Ptr<Key>, bool);
WEB_API WebIDL::ExceptionOr<GC::Ref<IDBKeyRange>> convert_a_value_to_a_key_range(JS::Realm&, Optional<JS::Value>, bool = false);
WEB_API JS::Value count_the_records_in_a_range(RecordSource, GC::Ref<IDBKeyRange>);
WEB_API WebIDL::ExceptionOr<JS::Value> retrieve_a_value_from_an_object_store(JS::Realm&, GC::Ref<ObjectStore>, GC::Ref<IDBKeyRange>);
WEB_API GC::Ptr<IDBCursor> iterate_a_cursor(JS::Realm&, GC::Ref<IDBCursor>, GC::Ptr<Key> = nullptr, GC::Ptr<Key> = nullptr, u64 = 1);
WEB_API JS::Value clear_an_object_store(GC::Ref<ObjectStore>);
WEB_API JS::Value retrieve_a_key_from_an_object_store(JS::Realm&, GC::Ref<ObjectStore>, GC::Ref<IDBKeyRange>);
WEB_API GC::Ref<JS::Array> retrieve_multiple_values_from_an_object_store(JS::Realm&, GC::Ref<ObjectStore>, GC::Ref<IDBKeyRange>, Optional<WebIDL::UnsignedLong>);
WEB_API GC::Ref<JS::Array> retrieve_multiple_keys_from_an_object_store(JS::Realm&, GC::Ref<ObjectStore>, GC::Ref<IDBKeyRange>, Optional<WebIDL::UnsignedLong>);
WEB_API JS::Value retrieve_a_referenced_value_from_an_index(JS::Realm&, GC::Ref<Index>, GC::Ref<IDBKeyRange>);
WEB_API JS::Value retrieve_a_value_from_an_index(JS::Realm&, GC::Ref<Index>, GC::Ref<IDBKeyRange>);
WEB_API GC::Ref<JS::Array> retrieve_multiple_referenced_values_from_an_index(JS::Realm&, GC::Ref<Index>, GC::Ref<IDBKeyRange>, Optional<WebIDL::UnsignedLong>);
WEB_API GC::Ref<JS::Array> retrieve_multiple_values_from_an_index(JS::Realm&, GC::Ref<Index>, GC::Ref<IDBKeyRange>, Optional<WebIDL::UnsignedLong>);
WEB_API void queue_a_database_task(GC::Ref<GC::Function<void()>>);
WEB_API bool cleanup_indexed_database_transactions(GC::Ref<HTML::EventLoop>);

}
