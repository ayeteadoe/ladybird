/*
 * Copyright (c) 2023, Kenneth Myhra <kennethmyhra@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Forward.h>
#include <LibWeb/XHR/FormData.h>

namespace Web::HTML {

struct SerializedFormData {
    String boundary;
    ByteBuffer serialized_data;
};

WEB_API WebIDL::ExceptionOr<XHR::FormDataEntry> create_entry(JS::Realm& realm, String const& name, Variant<GC::Ref<FileAPI::Blob>, String> const& value, Optional<String> const& filename = {});
WEB_API WebIDL::ExceptionOr<Optional<Vector<XHR::FormDataEntry>>> construct_entry_list(JS::Realm&, HTMLFormElement&, GC::Ptr<HTMLElement> submitter = nullptr, Optional<String> encoding = Optional<String> {});
WEB_API ErrorOr<String> normalize_line_breaks(StringView value);
WEB_API ErrorOr<SerializedFormData> serialize_to_multipart_form_data(Vector<XHR::FormDataEntry> const& entry_list);

}
