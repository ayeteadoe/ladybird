# FIXME: FOr Windows, implement a DirectX backend (Vulkan is also supported)
if (ENABLE_WEBGPUNATIVE_VULKAN_IMPL)
    include(vulkan)
    if (NOT HAS_VULKAN)
        message(FATAL_ERROR "Vulkan not found")
    endif()
    set(SOURCES
        Vulkan/Instance.cpp
        Vulkan/InstanceImpl.cpp
    )
elseif (ENABLE_WEBGPUNATIVE_METAL_IMPL)
    set(SOURCES
        Metal/Instance.cpp
    )
else()
    message(FATAL_ERROR "No backend implementation for WebGPUNative selected")
endif()

serenity_lib(LibWebGPUNative webgpunative)
target_compile_definitions(LibWebGPUNative PRIVATE $<$<CONFIG:Debug>:WEBGPUNATIVE_DEBUG>)
target_link_libraries(LibWebGPUNative PRIVATE AK)
if (ENABLE_WEBGPUNATIVE_VULKAN_IMPL)
    target_link_libraries(LibWebGPUNative PRIVATE Vulkan::Vulkan)
elseif (ENABLE_WEBGPUNATIVE_METAL_IMPL)
    target_link_libraries(LibWebGPUNative PRIVATE
        "-framework Foundation"
        "-framework Metal"
        "-framework MetalKit"
    )
endif()
