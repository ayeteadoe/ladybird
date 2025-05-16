
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/GPUDevice.h")
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/GPUDevice.cpp")

set(SOURCES ${SOURCES} PARENT_SCOPE)

target_sources(LibWeb
    PRIVATE
        GPUInstance.h
        GPUInstance.cpp
        GPUSurface.h
        GPUSurface.cpp
        GPUAdapter.h
        GPUAdapter.cpp
        GPUDevice.h
        GPUDevice.cpp
)
target_include_directories(LibWeb
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/webgpu/include" 
)
target_link_directories(LibWeb
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/webgpu/lib"
)
target_link_libraries(LibWeb
    PRIVATE
        libwgpu_native.so
)
