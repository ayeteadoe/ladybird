# TODO: Plan, create wrappers around https://github.com/gfx-rs/wgpu-native and https://github.com/google/dawn so we can
#   evaluate which API ladybird wants to use. Start with a wgpu impl, then try dawn

# Message in `#graphics` channel:
#[===[
[STREAMLINED VERSION]
It looked like WebGPU hadn't been discussed here for awhile, so I thought I'd take a stab at experimenting with it! 
Currently uses [wgpu-native](https://github.com/gfx-rs/wgpu-native/) for the Native layer, but I'm setting things up such that other backends can be used as well 
(e.g. Googles Dawn, or any others if anyone could recommend) so we can evaluate which which one we like best 
(ergonomically, performance, maintainability, etc). My current plan to stress test both the draft spec and my implementation 
is to create a simple WebGPU-based Scene Graph library that can load in 2D/3D primitives as well as arbitrary Wavefront OBJs (with optional .mtl sidecars) into the scene and add a basic camera for navigation

[LONG VERSION]
It looks like WebGPU hasn't been discussed here for awhile, so I'm assuming nobody else is currently/planning on working on this at the moment.
But if you are/interested let me know!

Given that we're using things like skia and the past discussion I read in this channel, my assumption is we don't want
our own custom Vulkan/Metal/DirectX/etc Native implementation, but to use something like
[wgpu-native](https://github.com/gfx-rs/wgpu-native) or [dawn](https://github.com/google/dawn). So I've started
experimenting with creating a `Web::WebGPU::Native` abstraction that will allow one to slot in either or any other third
party WebGPU native implementations so we can evaluate which one we like best (ergonomically, performance,
maintainability, etc). I've started with wgpu-native, and the attached demo you're seeing is a test bed I've temporarily
(obviously not wanting to submit any of this as-is) tacked onto the Qt Chrome that is rendering into a QWindow.

The idea is that this demo is the basis for an experimental WebGPUWidget in the Qt Chrome, that after initial feedback could be
refactored into LibWeb/WebGPU as a [GPUCanvasContext](https://developer.mozilla.org/en-US/docs/Web/API/GPUCanvasContext).
That would then allow anyone interested to start implementing the WebGPU IDL so we can actually use javascript instead of the Native API to render graphics

Here is the branch if you're interested in what I have so far. This is all a WIP but interested in getting early feedback on this
approach as if you strongly dislike it and/or have other plans then I won't waste time going down the wrong path.

]===]


# NOTE: For debugging the console, see https://github.com/LadybirdBrowser/ladybird/blob/master/Documentation/DevTools.md

# TODO: Initial things to implement to build up enough API to do the example in https://github.com/gfx-rs/wgpu-native/blob/trunk/examples/triangle/main.c:
# TODO: Once we have the Native version working, look at WebGL2 as a guide and find a simple example we can debug to find out how to implement GPUCanvasContext ourselves
#   - Native/GPUAdapter
#   - Native/GPUDevice
#   - Native/GPUQueue
#   - Native/GPUPipelineLayout
#   - Native/GPUSurfaceCapabilities
#   - Native/GPURenderPipeline
#   - Native/GPUSurfaceConfiguration
#   - Native/ShaderModule (https://developer.mozilla.org/en-US/docs/Web/API/GPUShaderModule)
#   - For the webgpu impl, extend shader module creation to take an in memory buffer as well (non-standard, but IMO should be in the spec).
#   -  Base it off of https://github.com/gfx-rs/wgpu-native/blob/trunk/examples/framework/framework.c#L34-L85

#  - Native/GPURenderPipeline[Config] (https://developer.mozilla.org/en-US/docs/Web/API/GPURenderPipeline)
#      - where Native/Vulkan/GPURenderPipeline[Config] is the actual initial implementation and also contains RenderPipelineConfig
#  - Native/

# TODO: Add CMakeLists.txt subdirectory to isolate all the WebGPU infrastructure.

target_sources(LibWeb
    PRIVATE
        GPU.h
        GPUAdapter.h
        GPUDevice.h
)

if (ENABLE_EXPERIMENTAL_WGPU_WEBGPU_IMPL)
    add_subdirectory(WGPU)
endif()
