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
    return adopt_own(*new NativeDrawingBuffer(Impl { .m_surface = surface, .m_shared_texture_memory = shared_texture_memory, .m_shared_texture_memory_properties = shared_texture_memory_properties, .m_texture = texture }));
}

RefPtr<Gfx::PaintingSurface> NativeDrawingBuffer::surface() const
{
    return m_impl->m_surface;
}

}
