target_sources(LibWeb
    PRIVATE
        GPU.h
        #GPUAdapter.h
        #GPUDevice.h
)


if (ENABLE_EXPERIMENTAL_WGPU_WEBGPU_IMPL)
    add_subdirectory(WGPU)
else()
    add_subdirectory(Stub)
endif()
