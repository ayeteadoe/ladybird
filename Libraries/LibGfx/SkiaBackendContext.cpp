/*
 * Copyright (c) 2024, Aliaksandr Kalenik <kalenik.aliaksandr@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/HashMap.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/RefPtr.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/SkiaBackendContext.h>

#include <core/SkSurface.h>
#include <gpu/ganesh/GrDirectContext.h>
#include <gpu/ganesh/GrBackendSemaphore.h>

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

#if defined(AK_OS_WINDOWS)
#    include <LibGfx/Direct3DContext.h>
#    include <gpu/ganesh/d3d/GrD3DBackendContext.h>

#    include <d3d11_4.h>
#    include <d3d12.h>
#    include <winrt/base.h>
#endif

namespace Gfx {

SkiaBackendContext::~SkiaBackendContext() = default;

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

    void flush_and_submit(SkSurface* surface) override
    {
        GrFlushInfo const flush_info {};
        m_context->flush(surface, SkSurfaces::BackendSurfaceAccess::kPresent, flush_info);
        m_context->submit(GrSyncCpu::kYes);
    }

    skgpu::VulkanExtensions const* extensions() const { return m_extensions.ptr(); }

    GrDirectContext* sk_context() const override { return m_context.get(); }

    VulkanContext const& vulkan_context() override { return m_vulkan_context; }

    MetalContext& metal_context() override { VERIFY_NOT_REACHED(); }

    Direct3DContext const& direct3d_context() override { VERIFY_NOT_REACHED(); }

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

    void flush_and_submit(SkSurface* surface) override
    {
        GrFlushInfo const flush_info {};
        m_context->flush(surface, SkSurfaces::BackendSurfaceAccess::kPresent, flush_info);
        m_context->submit(GrSyncCpu::kYes);
    }

    GrDirectContext* sk_context() const override { return m_context.get(); }

    VulkanContext const& vulkan_context() override { VERIFY_NOT_REACHED(); }

    MetalContext& metal_context() override { return m_metal_context; }

    Direct3DContext const& direct3d_context() override { VERIFY_NOT_REACHED(); }

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

#if defined(AK_OS_WINDOWS)

class SkiaDirect3DBackendContext final : public SkiaBackendContext {
    AK_MAKE_NONCOPYABLE(SkiaDirect3DBackendContext);
    AK_MAKE_NONMOVABLE(SkiaDirect3DBackendContext);

public:
    SkiaDirect3DBackendContext(sk_sp<GrDirectContext> context, NonnullOwnPtr<Direct3DContext> direct3d_context)
        : m_context(move(context))
        , m_direct3d_context(move(direct3d_context))
    {
    }

    ~SkiaDirect3DBackendContext() override = default;

    void flush_and_submit(SkSurface* surface) override
    {
        GrFlushInfo const flush_info {};
        m_context->flush(surface, SkSurfaces::BackendSurfaceAccess::kPresent, flush_info);
        m_context->submit(GrSyncCpu::kYes);
    }

    GrDirectContext* sk_context() const override { return m_context.get(); }

    VulkanContext const& vulkan_context() override { VERIFY_NOT_REACHED(); }

    MetalContext& metal_context() override { VERIFY_NOT_REACHED(); }

    Direct3DContext const& direct3d_context() override { return *m_direct3d_context; }

    ID3D12Fence& add_exported_fence(SkSurface& surface, winrt::com_ptr<ID3D12Fence> d12_fence)
    {
        return *m_exported_fences.ensure(&surface, [d12_fence = move(d12_fence)] { return d12_fence; }).get();
    }

private:
    sk_sp<GrDirectContext> m_context;
    NonnullOwnPtr<Direct3DContext> m_direct3d_context;
    // FIXME: Allow multiple fences to be associated with a single surface
    HashMap<SkSurface*, winrt::com_ptr<ID3D12Fence>> m_exported_fences;
};

RefPtr<SkiaBackendContext> SkiaBackendContext::create_direct3d_context(NonnullOwnPtr<Direct3DContext> direct3d_context)
{
    GrD3DBackendContext backend_context;
    backend_context.fAdapter = gr_cp(&direct3d_context->adapter());
    backend_context.fDevice = gr_cp(&direct3d_context->d12_device());
    backend_context.fQueue = gr_cp(&direct3d_context->d12_command_queue());
    sk_sp<GrDirectContext> ctx = GrDirectContext::MakeDirect3D(backend_context);
    VERIFY(ctx);
    return adopt_ref(*new SkiaDirect3DBackendContext(ctx, move(direct3d_context)));
}

Optional<ID3D12Fence&> SkiaBackendContext::open_shared_fence(SkSurface& surface, ID3D11Fence& shared_d11_fence)
{
    HANDLE shared_fence_handle = INVALID_HANDLE_VALUE;
    if (HRESULT const hr = shared_d11_fence.CreateSharedHandle(nullptr, GENERIC_READ | GENERIC_WRITE, nullptr, &shared_fence_handle); FAILED(hr)) {
        dbgln("CreateSharedHandle failed: {}", Error::from_windows_error(hr));
        return {};
    }

    winrt::com_ptr<ID3D12Fence> d12_fence;
    if (HRESULT const hr = direct3d_context().d12_device().OpenSharedHandle(shared_fence_handle, IID_PPV_ARGS(d12_fence.put())); FAILED(hr)) {
        dbgln("OpenSharedHandle failed: {}", Error::from_windows_error(hr));
        return {};
    }

    auto d3d_ctx = static_cast<SkiaDirect3DBackendContext*>(this);
    d3d_ctx->add_exported_fence(surface, move(d12_fence));

    dbgln("Created backend semaphore for {}", &surface);
    return d3d_ctx->add_exported_fence(surface, move(d12_fence));
}

#endif

}
