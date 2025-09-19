/*
 * Copyright (c) 2024, Aliaksandr Kalenik <kalenik.aliaksandr@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

// NOTE: Need to include before any AK headers as SkCPURecorder has a method TODO() that conflicts with the Ak/Assertions.h TODO macro
#include <gpu/graphite/Context.h>
#include <gpu/graphite/ContextOptions.h>
#include <gpu/graphite/Recorder.h>
#include <gpu/graphite/dawn/DawnBackendContext.h>

#include <AK/NonnullOwnPtr.h>
#include <AK/RefPtr.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/SkiaBackendContext.h>

#include <core/SkSurface.h>
#include <gpu/ganesh/GrDirectContext.h>

#ifdef USE_VULKAN
#    include <gpu/ganesh/vk/GrVkDirectContext.h>
#    include <gpu/vk/VulkanBackendContext.h>
#    include <gpu/vk/VulkanExtensions.h>
#endif

#ifdef AK_OS_MACOS
#    include <gpu/ganesh/GrBackendSurface.h>
#    include <gpu/ganesh/mtl/GrMtlBackendContext.h>
#    include <gpu/ganesh/mtl/GrMtlBackendSurface.h>
#    include <gpu/ganesh/mtl/GrMtlDirectContext.h>
#endif

namespace Gfx {

#ifdef USE_VULKAN
class SkiaVulkanBackendContext final : public SkiaBackendContext {
    AK_MAKE_NONCOPYABLE(SkiaVulkanBackendContext);
    AK_MAKE_NONMOVABLE(SkiaVulkanBackendContext);

public:
    SkiaVulkanBackendContext(sk_sp<GrDirectContext> context, VulkanContext const& vulkan_context, NonnullOwnPtr<skgpu::VulkanExtensions> extensions)
        : m_context(move(context))
        , m_extensions(move(extensions))
        , m_vulkan_context(vulkan_context)
    {
    }

    ~SkiaVulkanBackendContext() override { }

    Type type() const override { return Type::Ganesh; }

    void flush_and_submit(SkSurface* surface) override
    {
        GrFlushInfo const flush_info {};
        m_context->flush(surface, SkSurfaces::BackendSurfaceAccess::kPresent, flush_info);
        m_context->submit(GrSyncCpu::kYes);
    }

    skgpu::VulkanExtensions const* extensions() const { return m_extensions.ptr(); }

    GrDirectContext* sk_ganesh_context() const override { return m_context.get(); }

    VulkanContext const& vulkan_context() override { return m_vulkan_context; }

    MetalContext& metal_context() override { VERIFY_NOT_REACHED(); }

private:
    sk_sp<GrDirectContext> m_context;
    NonnullOwnPtr<skgpu::VulkanExtensions> m_extensions;
    VulkanContext const m_vulkan_context;
};

RefPtr<SkiaBackendContext> SkiaBackendContext::create_vulkan_context(VulkanContext const& vulkan_context)
{
    skgpu::VulkanBackendContext backend_context;

    backend_context.fInstance = vulkan_context.instance;
    backend_context.fDevice = vulkan_context.logical_device;
    backend_context.fQueue = vulkan_context.graphics_queue;
    backend_context.fGraphicsQueueIndex = vulkan_context.graphics_queue_family;
    backend_context.fPhysicalDevice = vulkan_context.physical_device;
    backend_context.fMaxAPIVersion = vulkan_context.api_version;
    backend_context.fGetProc = [](char const* proc_name, VkInstance instance, VkDevice device) {
        if (device != VK_NULL_HANDLE) {
            return vkGetDeviceProcAddr(device, proc_name);
        }
        return vkGetInstanceProcAddr(instance, proc_name);
    };

    auto extensions = make<skgpu::VulkanExtensions>();
    backend_context.fVkExtensions = extensions.ptr();

    sk_sp<GrDirectContext> ctx = GrDirectContexts::MakeVulkan(backend_context);
    VERIFY(ctx);
    return adopt_ref(*new SkiaVulkanBackendContext(ctx, vulkan_context, move(extensions)));
}
#endif

#ifdef AK_OS_MACOS
class SkiaMetalBackendContext final : public SkiaBackendContext {
    AK_MAKE_NONCOPYABLE(SkiaMetalBackendContext);
    AK_MAKE_NONMOVABLE(SkiaMetalBackendContext);

public:
    SkiaMetalBackendContext(sk_sp<GrDirectContext> context, NonnullRefPtr<MetalContext> metal_context)
        : m_context(move(context))
        , m_metal_context(move(metal_context))
    {
    }

    ~SkiaMetalBackendContext() override { }

    Type type() const override { return Type::Ganesh; }

    void flush_and_submit(SkSurface* surface) override
    {
        GrFlushInfo const flush_info {};
        m_context->flush(surface, SkSurfaces::BackendSurfaceAccess::kPresent, flush_info);
        m_context->submit(GrSyncCpu::kYes);
    }

    GrDirectContext* sk_ganesh_context() const override { return m_context.get(); }

    VulkanContext const& vulkan_context() override { VERIFY_NOT_REACHED(); }

    MetalContext& metal_context() override { return m_metal_context; }

private:
    sk_sp<GrDirectContext> m_context;
    NonnullRefPtr<MetalContext> m_metal_context;
};

RefPtr<SkiaBackendContext> SkiaBackendContext::create_metal_context(NonnullRefPtr<MetalContext> metal_context)
{
    GrMtlBackendContext backend_context;
    backend_context.fDevice.retain(metal_context->device());
    backend_context.fQueue.retain(metal_context->queue());
    sk_sp<GrDirectContext> ctx = GrDirectContexts::MakeMetal(backend_context);
    return adopt_ref(*new SkiaMetalBackendContext(move(ctx), move(metal_context)));
}
#endif

class SkiaDawnBackendContext final : public SkiaBackendContext {
    AK_MAKE_NONCOPYABLE(SkiaDawnBackendContext);
    AK_MAKE_NONMOVABLE(SkiaDawnBackendContext);

public:
    SkiaDawnBackendContext(std::unique_ptr<skgpu::graphite::Context> context)
        : m_dawn_context(move(context))
    {
        VERIFY(m_dawn_context);

        skgpu::graphite::RecorderOptions recorder_options;
        m_dawn_recorder = m_dawn_context->makeRecorder(recorder_options);
        VERIFY(m_dawn_recorder);
    }

    ~SkiaDawnBackendContext() override { }

    Type type() const override { return Type::Graphite; }

    void flush_and_submit(SkSurface*) override
    {
        if (std::unique_ptr<skgpu::graphite::Recording> recording = m_dawn_recorder->snap()) {
            // FIXME: Read doc for what the other fields allow if that helps in the WebGPU scenario
            skgpu::graphite::InsertRecordingInfo insert_recording_info { .fRecording = recording.get() };
            m_dawn_context->insertRecording(insert_recording_info);
            m_dawn_context->submit(skgpu::graphite::SyncToCpu::kYes);
        }
    }

    skgpu::graphite::Context* sk_graphite_context() const override
    {
        return m_dawn_context.get();
    }

    skgpu::graphite::Recorder* sk_graphite_recorder() const override { return m_dawn_recorder.get(); }

    VulkanContext const& vulkan_context() override { VERIFY_NOT_REACHED(); }

    MetalContext& metal_context() override { VERIFY_NOT_REACHED(); }

private:
    std::unique_ptr<skgpu::graphite::Context> m_dawn_context;
    std::unique_ptr<skgpu::graphite::Recorder> m_dawn_recorder;
};

RefPtr<SkiaBackendContext> SkiaBackendContext::create_dawn_context(skgpu::graphite::DawnBackendContext& dawn_context)
{
    skgpu::graphite::ContextOptions context_options;
    std::unique_ptr<skgpu::graphite::Context> ctx = skgpu::graphite::ContextFactory::MakeDawn(dawn_context, context_options);
    return adopt_ref(*new SkiaDawnBackendContext(move(ctx)));
}

}
