/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/PaintingSurface.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebGPU/Native/NativeGPUDevice.h>
#include <LibWeb/WebGPU/Native/WebGPUNativeMacros.h>

namespace Web::WebGPU {

class NativeDrawingBuffer {
    WEBGPU_NATIVE_DECLARE_SPECIAL_MEMBERS(NativeDrawingBuffer);
    WEBGPU_NATIVE_DECLARE_PIMPL(NativeDrawingBuffer);

public:
    static ErrorOr<NonnullOwnPtr<NativeDrawingBuffer>> create(NonnullRefPtr<Gfx::SkiaBackendContext> const&, NativeGPUDevice const&, Gfx::IntSize const&);

    RefPtr<Gfx::PaintingSurface> surface() const;
};

}
