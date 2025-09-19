/*
 * Copyright (c) 2024, Aliaksandr Kalenik <kalenik.aliaksandr@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/AtomicRefCounted.h>
#include <AK/Noncopyable.h>
#include <LibThreading/Mutex.h>

#ifdef USE_VULKAN
#    include <LibGfx/VulkanContext.h>
#endif

#ifdef AK_OS_MACOS
#    include <LibGfx/MetalContext.h>
#endif

class GrDirectContext;
class SkSurface;

namespace skgpu::graphite {

struct DawnBackendContext;
class Context;
class Recorder;

}

namespace Gfx {

struct VulkanContext;
class MetalContext;

class SkiaBackendContext : public AtomicRefCounted<SkiaBackendContext> {
    AK_MAKE_NONCOPYABLE(SkiaBackendContext);
    AK_MAKE_NONMOVABLE(SkiaBackendContext);

public:
    enum Type {
        None,
        Ganesh,
        Graphite
    };

#ifdef USE_VULKAN
    static RefPtr<SkiaBackendContext> create_vulkan_context(const VulkanContext& vulkan_context);
#endif

#ifdef AK_OS_MACOS
    static RefPtr<SkiaBackendContext> create_metal_context(NonnullRefPtr<MetalContext>);
#endif

    static RefPtr<SkiaBackendContext> create_dawn_context(skgpu::graphite::DawnBackendContext&);

    SkiaBackendContext() { }
    virtual ~SkiaBackendContext() { }

    virtual Type type() const { return Type::None; }

    virtual void flush_and_submit(SkSurface*) { }

    virtual GrDirectContext* sk_ganesh_context() const { VERIFY_NOT_REACHED(); }

    virtual skgpu::graphite::Context* sk_graphite_context() const { VERIFY_NOT_REACHED(); }

    virtual skgpu::graphite::Recorder* sk_graphite_recorder() const { VERIFY_NOT_REACHED(); }

    virtual MetalContext& metal_context() = 0;
    virtual VulkanContext const& vulkan_context() = 0;

    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }

private:
    Threading::Mutex m_mutex;
};

}
