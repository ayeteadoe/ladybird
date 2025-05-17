# TODO: Create a tab widget that itself has a tab widget for all our tests. Start with Clear screen, then to the
#   rotating Triangle, then do the rotating texture. Basically just do my *VK demo app examples but using the WebGPU API

target_sources(ladybird
    PRIVATE
        WebGPUWindow.h
        WebGPUWindow.cpp
        # TODO: WebGPUTriangle.h/.cpp
        WebGPUTabWidget.h
        WebGPUTabWidget.cpp
        shaders.qrc
)
target_compile_definitions(ladybird
    PRIVATE
        EXPERIMENTAL_WEBGPU_WGPU_IMPL
)


target_include_directories(ladybird
    PRIVATE
        ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
)
