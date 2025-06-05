if (NOT ENABLE_WEBGPUNATIVE)
    message(WARNING "No backend implementation for WebGPUNative available")
    return()
endif()

if (ENABLE_WEBGPUNATIVE_VULKAN_IMPL)
    include(vulkan)
    if (NOT HAS_VULKAN)
        message(FATAL_ERROR "Vulkan not found")
    endif()
    set(SOURCES
        Vulkan/Instance.cpp
        Vulkan/InstanceImpl.cpp
    )
endif()

serenity_lib(LibWebGPUNative webgpunative EXPLICIT_SYMBOL_EXPORT)
target_compile_definitions(LibWebGPUNative PRIVATE $<$<CONFIG:Debug>:WEBGPUNATIVE_DEBUG>)
target_link_libraries(LibWebGPUNative PRIVATE AK)
if (ENABLE_WEBGPUNATIVE_VULKAN_IMPL)
    target_link_libraries(LibWebGPUNative PRIVATE Vulkan::Vulkan Vulkan::UtilityHeaders vk-bootstrap::vk-bootstrap)
endif()
