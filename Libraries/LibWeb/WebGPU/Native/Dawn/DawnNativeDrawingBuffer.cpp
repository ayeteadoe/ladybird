/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/Native/Dawn/DawnNativeDrawingBuffer.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUDevice.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeDrawingBuffer);

ErrorOr<NonnullOwnPtr<NativeDrawingBuffer>> NativeDrawingBuffer::create([[maybe_unused]] NonnullRefPtr<Gfx::SkiaBackendContext> const& skia_backend_context, NativeGPUDevice const& native_gpu_device, [[maybe_unused]] Gfx::IntSize const& size)
{
    wgpu::SharedTextureMemoryDescriptor shared_texture_memory_descriptor {};
    RefPtr<Gfx::PaintingSurface> surface {};

#if defined(USE_VULKAN_IMAGES)
    // FIXME: Populate wgpu::SharedTextureMemoryDmaBufDescriptor or wgpu::SharedTextureMemoryOpaqueFDDescriptor for Vulkan-based SkiaBackendContext on Linux
#elif defined(AK_OS_MACOS)
    auto iosurface = Core::IOSurfaceHandle::create(size.width(), size.height());
    surface = Gfx::PaintingSurface::create_from_iosurface(move(iosurface), skia_backend_context, Gfx::PaintingSurface::Origin::TopLeft);

    wgpu::SharedTextureMemoryIOSurfaceDescriptor shared_texture_memory_io_surface_descriptor {};
    shared_texture_memory_io_surface_descriptor.ioSurface = iosurface.core_foundation_pointer();
    shared_texture_memory_descriptor.nextInChain = &shared_texture_memory_io_surface_descriptor;
#elif defined(AK_OS_WINDOWS)
    // FIXME: Populate wgpu::SharedTextureMemoryDXGISharedHandleDescriptor for DirectX-based SkiaBackendContext on Windows
#endif
    if (surface == nullptr)
        return Error::from_string_literal("Unable to painting surface from shared texture memory");

    auto shared_texture_memory = native_gpu_device.m_impl->m_device.ImportSharedTextureMemory(&shared_texture_memory_descriptor);
    wgpu::SharedTextureMemoryProperties shared_texture_memory_properties {};
    auto const shared_texture_memory_properties_result = shared_texture_memory.GetProperties(&shared_texture_memory_properties);
    if (shared_texture_memory_properties_result != wgpu::Status::Success)
        return Error::from_string_literal("Unable to create shared texture memory");

    wgpu::TextureDescriptor texture_descriptor {};
    texture_descriptor.usage = shared_texture_memory_properties.usage;
    texture_descriptor.format = shared_texture_memory_properties.format;
    texture_descriptor.size = shared_texture_memory_properties.size;
    texture_descriptor.dimension = wgpu::TextureDimension::e2D;
    wgpu::Texture texture = shared_texture_memory.CreateTexture(&texture_descriptor);
    if (texture == nullptr)
        return Error::from_string_literal("Unable to create texture from shared texture memory");

    wgpu::SharedTextureMemoryBeginAccessDescriptor shared_texture_memory_begin_access_descriptor {};
    shared_texture_memory_begin_access_descriptor.initialized = false;
    // FIXME: Expose a begin_access() method so the GPUCanvasContext has full control over texture access/synchronization
    // FIXME: Synchronize with shared fences, as the Dawn device and the Skia device are both using the same texture memory
    //        See https://docs.google.com/document/d/1uRGL6vE1mSbpWd2v_KU5--RT5EjTXtruwiC7Ri3ZKz4/edit?tab=t.0#heading=h.953a4uj4vvwh for details.
    //        In our case, to start we can create the platform specific shared fence abstraction ourselves and import it into Dawn instead of exporting
    //        from Dawn. See https://docs.google.com/document/d/1uRGL6vE1mSbpWd2v_KU5--RT5EjTXtruwiC7Ri3ZKz4/edit?tab=t.0#heading=h.gsf8tktx1v6j for details
    //        on imported handle signalling strategies. In terms of intergrating the synchronization into Skia, we can look at populating the GrFlushInfo with
    //        a GrBackendSemaphore that is signaled when all skia commands have been issued. For Metal, GrBackendSemaphores::MakeMetal() takes in a GrMTLHandle
    //        which should be compatible with the MTLSharedEvent required for wgpu::SharedFenceMTLSharedEventDescriptor. For Vulkan, GrBackendSemaphores::MakeVk()
    //        takes in a VkSemaphore which is required for wgpu::SharedFenceVkSemaphore*Descriptor. For Direct3D, GrBackendSemaphore::initDirect3D  takes in a
    //        wrapper to a ID3D12Fence which can be used to create a shared handle required for wgpu::SharedFenceDXGISharedHandleDescriptor via ID3D12Fence::CreateSharedHandle().
    auto const shared_texture_memory_begin_access_result = shared_texture_memory.BeginAccess(texture, &shared_texture_memory_begin_access_descriptor);
    if (shared_texture_memory_begin_access_result != wgpu::Status::Success)
        return Error::from_string_literal("Unable to begin shared texture memory access");

    return adopt_own(*new NativeDrawingBuffer(Impl { .m_surface = surface, .m_shared_texture_memory = shared_texture_memory, .m_shared_texture_memory_properties = shared_texture_memory_properties, .m_texture = texture }));
}

RefPtr<Gfx::PaintingSurface> NativeDrawingBuffer::surface() const
{
    return m_impl->m_surface;
}

ErrorOr<void> NativeDrawingBuffer::end_access()
{
    // FIXME: Synchronize with shared fences.
    wgpu::SharedTextureMemoryEndAccessState shared_texture_memory_end_access_state {};
    auto const shared_texture_memory_end_access_result = m_impl->m_shared_texture_memory.EndAccess(m_impl->m_texture, &shared_texture_memory_end_access_state);
    if (shared_texture_memory_end_access_result != wgpu::Status::Success)
        return Error::from_string_literal("Unable to end shared texture memory access");
    return {};
}

}
