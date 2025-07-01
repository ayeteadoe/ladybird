/*
 * Copyright (c) 2024, Jelle Raaijmakers <jelle@ladybird.org>
 * Copyright (c) 2024, Luke Wilde <luke@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Forward.h>
#include <LibWeb/WebGL/WebGLObject.h>

namespace Web::WebGL {

class WEB_API WebGLRenderbuffer final : public WebGLObject {
    WEB_PLATFORM_OBJECT(WebGLRenderbuffer, WebGLObject);
    GC_DECLARE_ALLOCATOR(WebGLRenderbuffer);

public:
    static GC::Ref<WebGLRenderbuffer> create(JS::Realm& realm, WebGLRenderingContextBase&, GLuint handle);

    virtual ~WebGLRenderbuffer();

protected:
    explicit WebGLRenderbuffer(JS::Realm&, WebGLRenderingContextBase&, GLuint handle);

    virtual void initialize(JS::Realm&) override;
};

}
