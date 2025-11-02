/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/GPUDeviceLostInfoPrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>

namespace Web::WebGPU {

class GPUDeviceLostInfo final : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(GPUDeviceLostInfo, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(GPUDeviceLostInfo);

    static JS::ThrowCompletionOr<GC::Ref<GPUDeviceLostInfo>> create(JS::Realm&, Bindings::GPUDeviceLostReason, String const&);

    Bindings::GPUDeviceLostReason reason() const { return m_reason; }

    String const& message() const { return m_message; }

private:
    explicit GPUDeviceLostInfo(JS::Realm&, Bindings::GPUDeviceLostReason, String const&);

    void initialize(JS::Realm&) override;

    void visit_edges(Visitor&) override;

    Bindings::GPUDeviceLostReason m_reason;
    String m_message;
};

}
