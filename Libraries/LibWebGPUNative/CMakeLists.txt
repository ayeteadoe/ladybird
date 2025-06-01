# FIXME: FOr Windows, implement a DirectX backend (Vulkan is also supported)
if (ENABLE_WEBGPUNATIVE_VULKAN_IMPL)
    include(vulkan)
    if (NOT HAS_VULKAN)
        message(FATAL_ERROR "Vulkan not found")
    endif()
    set(SOURCES
        Vulkan/Adapter.cpp
        Vulkan/AdapterImpl.cpp
        Vulkan/CommandBuffer.cpp
        Vulkan/CommandBufferImpl.cpp
        Vulkan/CommandEncoder.cpp
        Vulkan/CommandEncoderImpl.cpp
        Vulkan/Device.cpp
        Vulkan/DeviceImpl.cpp
        Vulkan/Instance.cpp
        Vulkan/InstanceImpl.cpp
        Vulkan/Queue.cpp
        Vulkan/QueueImpl.cpp
        Vulkan/Texture.cpp
        Vulkan/TextureImpl.cpp
    )
elseif (ENABLE_WEBGPUNATIVE_METAL_IMPL)
    set(SOURCES
        Metal/Adapter.cpp
        Metal/AdapterImpl.mm
        Metal/CommandBuffer.cpp
        Metal/CommandBufferImpl.mm
        Metal/CommandEncoder.cpp
        Metal/CommandEncoderImpl.mm
        Metal/Device.cpp
        Metal/DeviceImpl.mm
        Metal/Handle.mm
        Metal/Instance.cpp
        Metal/Queue.cpp
        Metal/QueueImpl.mm
        Metal/Texture.cpp
        Metal/TextureImpl.mm
    )
else()
    message(FATAL_ERROR "No backend implementation for WebGPUNative selected")
endif()

serenity_lib(LibWebGPUNative webgpunative)
target_compile_definitions(LibWebGPUNative PRIVATE $<$<CONFIG:Debug>:WEBGPUNATIVE_DEBUG>)
target_link_libraries(LibWebGPUNative PRIVATE AK LibCore)
if (ENABLE_WEBGPUNATIVE_VULKAN_IMPL)
    target_link_libraries(LibWebGPUNative PRIVATE Vulkan::Vulkan)
elseif (ENABLE_WEBGPUNATIVE_METAL_IMPL)
    target_link_libraries(LibWebGPUNative PRIVATE
        "-framework Foundation"
        "-framework Metal"
    )
endif()
