/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/WebGPU/GPUBindGroupLayout.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/GPUTexture.h>
#include <LibWeb/WebGPU/GPUTextureView.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUBindGroup.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUBindGroupLayout.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUBuffer.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUCommandEncoder.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUDevice.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUQueue.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPURenderPipeline.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUShaderModule.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUTexture.h>
#include <LibWeb/WebGPU/Native/Dawn/DawnNativeGPUTextureView.h>

namespace Web::WebGPU {

WEBGPU_NATIVE_DEFINE_SPECIAL_MEMBERS(NativeGPUDevice);

NativeGPUQueue NativeGPUDevice::Impl::queue() const
{
    auto native_gpu_queue = NativeGPUQueue::create();
    native_gpu_queue.m_impl->m_queue = m_device.GetQueue();
    return native_gpu_queue;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createbuffer
NativeGPUBuffer NativeGPUDevice::Impl::create_buffer(GPUBufferDescriptor const& descriptor) const
{
    // FIXME: Implement specification

    auto buffer = NativeGPUBuffer::create();
    wgpu::BufferDescriptor buffer_descriptor {};
    buffer_descriptor.size = descriptor.size;
    buffer_descriptor.usage = static_cast<wgpu::BufferUsage>(descriptor.usage);
    buffer_descriptor.mappedAtCreation = descriptor.mapped_at_creation;
    buffer.m_impl->m_buffer = m_device.CreateBuffer(&buffer_descriptor);
    return buffer;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createbindgroup
NativeGPUBindGroup NativeGPUDevice::Impl::create_bind_group(GPUBindGroupDescriptor const& descriptor) const
{
    // FIXME: Implement specification

    auto bind_group = NativeGPUBindGroup::create();
    wgpu::BindGroupDescriptor bind_group_descriptor {};
    bind_group_descriptor.layout = descriptor.layout->native_gpu_bind_group_layout().m_impl->m_bind_group_layout;
    Vector<wgpu::BindGroupEntry> bind_group_entries {};
    for (auto const& entry : descriptor.entries) {
        wgpu::BindGroupEntry bind_group_entry {};
        bind_group_entry.binding = entry.binding;
        entry.resource.visit(
            [](GC::Root<GPUTexture> const&) {
                // FIXME: Dawn does not support setting a texture resource
            },
            [&](GC::Root<GPUTextureView> const& texture_view) {
                bind_group_entry.textureView = texture_view->native_gpu_texture_view().m_impl->m_texture_view;
            },
            [&](GC::Root<GPUBuffer> const& buffer) {
                bind_group_entry.buffer = buffer->native_gpu_buffer().m_impl->m_buffer;
            },
            [&](GPUBufferBinding const& buffer_binding) {
                bind_group_entry.offset = buffer_binding.offset;
                if (buffer_binding.size.has_value())
                    bind_group_entry.size = buffer_binding.size.value();
                bind_group_entry.buffer = buffer_binding.buffer->native_gpu_buffer().m_impl->m_buffer;
            },
            [](Empty) {});
        bind_group_entries.append(bind_group_entry);
    }
    bind_group_descriptor.entryCount = bind_group_entries.size();
    bind_group_descriptor.entries = bind_group_entries.data();
    bind_group.m_impl->m_bind_group = m_device.CreateBindGroup(&bind_group_descriptor);
    return bind_group;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createshadermodule
NativeGPUShaderModule NativeGPUDevice::Impl::create_shader_module(GPUShaderModuleDescriptor const& descriptor) const
{
    // FIXME: Implement specification

    auto shader_module = NativeGPUShaderModule::create();
    auto code_view = descriptor.code.bytes_as_string_view();
    wgpu::ShaderSourceWGSL shader_source_wgsl { wgpu::ShaderSourceWGSL::Init { .nextInChain = nullptr, .code = wgpu::StringView { code_view.characters_without_null_termination(), code_view.length() } } };
    wgpu::ShaderModuleDescriptor shader_module_descriptor { .nextInChain = &shader_source_wgsl };
    shader_module.m_impl->m_shader_module = m_device.CreateShaderModule(&shader_module_descriptor);
    return shader_module;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createrenderpipeline
NativeGPURenderPipeline NativeGPUDevice::Impl::create_render_pipeline(GPURenderPipelineDescriptor const& descriptor) const
{
    // FIXME: Implement specification

    auto render_pipeline = NativeGPURenderPipeline::create();

    wgpu::RenderPipelineDescriptor render_pipeline_descriptor {};

    wgpu::VertexState vertex_state {};
    vertex_state.module = descriptor.vertex.module->native_gpu_shader_module().m_impl->m_shader_module;
    if (descriptor.vertex.entry_point.has_value()) {
        auto entry_point = descriptor.vertex.entry_point.value().bytes_as_string_view();
        vertex_state.entryPoint = wgpu::StringView { entry_point.characters_without_null_termination(), entry_point.length() };
    }
    Vector<wgpu::VertexBufferLayout> vertex_buffer_layouts {};
    Vector<wgpu::VertexAttribute> vertex_attributes {};
    for (auto const& buffer : descriptor.vertex.buffers) {
        for (auto const& attribute : buffer.attributes) {
            wgpu::VertexAttribute vertex_attribute {};
            switch (attribute.format) {
            case Bindings::GPUVertexFormat::Uint8:
                vertex_attribute.format = wgpu::VertexFormat::Uint8;
                break;
            case Bindings::GPUVertexFormat::Uint8x2:
                vertex_attribute.format = wgpu::VertexFormat::Uint8x2;
                break;
            case Bindings::GPUVertexFormat::Uint8x4:
                vertex_attribute.format = wgpu::VertexFormat::Uint8x4;
                break;
            case Bindings::GPUVertexFormat::Sint8:
                vertex_attribute.format = wgpu::VertexFormat::Sint8;
                break;
            case Bindings::GPUVertexFormat::Sint8x2:
                vertex_attribute.format = wgpu::VertexFormat::Sint8x2;
                break;
            case Bindings::GPUVertexFormat::Sint8x4:
                vertex_attribute.format = wgpu::VertexFormat::Sint8x4;
                break;
            case Bindings::GPUVertexFormat::Unorm8:
                vertex_attribute.format = wgpu::VertexFormat::Unorm8;
                break;
            case Bindings::GPUVertexFormat::Unorm8x2:
                vertex_attribute.format = wgpu::VertexFormat::Unorm8x2;
                break;
            case Bindings::GPUVertexFormat::Unorm8x4:
                vertex_attribute.format = wgpu::VertexFormat::Unorm8x4;
                break;
            case Bindings::GPUVertexFormat::Snorm8:
                vertex_attribute.format = wgpu::VertexFormat::Snorm8;
                break;
            case Bindings::GPUVertexFormat::Snorm8x2:
                vertex_attribute.format = wgpu::VertexFormat::Snorm8x2;
                break;
            case Bindings::GPUVertexFormat::Snorm8x4:
                vertex_attribute.format = wgpu::VertexFormat::Snorm8x4;
                break;
            case Bindings::GPUVertexFormat::Uint16:
                vertex_attribute.format = wgpu::VertexFormat::Uint16;
                break;
            case Bindings::GPUVertexFormat::Uint16x2:
                vertex_attribute.format = wgpu::VertexFormat::Uint16x2;
                break;
            case Bindings::GPUVertexFormat::Uint16x4:
                vertex_attribute.format = wgpu::VertexFormat::Uint16x4;
                break;
            case Bindings::GPUVertexFormat::Sint16:
                vertex_attribute.format = wgpu::VertexFormat::Sint16;
                break;
            case Bindings::GPUVertexFormat::Sint16x2:
                vertex_attribute.format = wgpu::VertexFormat::Sint16x2;
                break;
            case Bindings::GPUVertexFormat::Sint16x4:
                vertex_attribute.format = wgpu::VertexFormat::Sint16x4;
                break;
            case Bindings::GPUVertexFormat::Unorm16:
                vertex_attribute.format = wgpu::VertexFormat::Unorm16;
                break;
            case Bindings::GPUVertexFormat::Unorm16x2:
                vertex_attribute.format = wgpu::VertexFormat::Unorm16x2;
                break;
            case Bindings::GPUVertexFormat::Unorm16x4:
                vertex_attribute.format = wgpu::VertexFormat::Unorm16x4;
                break;
            case Bindings::GPUVertexFormat::Snorm16:
                vertex_attribute.format = wgpu::VertexFormat::Snorm16;
                break;
            case Bindings::GPUVertexFormat::Snorm16x2:
                vertex_attribute.format = wgpu::VertexFormat::Snorm16x2;
                break;
            case Bindings::GPUVertexFormat::Snorm16x4:
                vertex_attribute.format = wgpu::VertexFormat::Snorm16x4;
                break;
            case Bindings::GPUVertexFormat::Float16:
                vertex_attribute.format = wgpu::VertexFormat::Float16;
                break;
            case Bindings::GPUVertexFormat::Float16x2:
                vertex_attribute.format = wgpu::VertexFormat::Float16x2;
                break;
            case Bindings::GPUVertexFormat::Float16x4:
                vertex_attribute.format = wgpu::VertexFormat::Float16x4;
                break;
            case Bindings::GPUVertexFormat::Float32:
                vertex_attribute.format = wgpu::VertexFormat::Float32;
                break;
            case Bindings::GPUVertexFormat::Float32x2:
                vertex_attribute.format = wgpu::VertexFormat::Float32x2;
                break;
            case Bindings::GPUVertexFormat::Float32x3:
                vertex_attribute.format = wgpu::VertexFormat::Float32x3;
                break;
            case Bindings::GPUVertexFormat::Float32x4:
                vertex_attribute.format = wgpu::VertexFormat::Float32x4;
                break;
            case Bindings::GPUVertexFormat::Uint32:
                vertex_attribute.format = wgpu::VertexFormat::Uint32;
                break;
            case Bindings::GPUVertexFormat::Uint32x2:
                vertex_attribute.format = wgpu::VertexFormat::Uint32x2;
                break;
            case Bindings::GPUVertexFormat::Uint32x3:
                vertex_attribute.format = wgpu::VertexFormat::Uint32x3;
                break;
            case Bindings::GPUVertexFormat::Uint32x4:
                vertex_attribute.format = wgpu::VertexFormat::Uint32x4;
                break;
            case Bindings::GPUVertexFormat::Sint32:
                vertex_attribute.format = wgpu::VertexFormat::Sint32;
                break;
            case Bindings::GPUVertexFormat::Sint32x2:
                vertex_attribute.format = wgpu::VertexFormat::Sint32x2;
                break;
            case Bindings::GPUVertexFormat::Sint32x3:
                vertex_attribute.format = wgpu::VertexFormat::Sint32x3;
                break;
            case Bindings::GPUVertexFormat::Sint32x4:
                vertex_attribute.format = wgpu::VertexFormat::Sint32x4;
                break;
            case Bindings::GPUVertexFormat::Unorm1010102:
                vertex_attribute.format = wgpu::VertexFormat::Unorm10_10_10_2;
                break;
            case Bindings::GPUVertexFormat::Unorm8x4Bgra:
                vertex_attribute.format = wgpu::VertexFormat::Unorm8x4BGRA;
                break;
            }
            vertex_attribute.offset = attribute.offset;
            vertex_attribute.shaderLocation = attribute.shader_location;
            vertex_attributes.append(vertex_attribute);
        }
        wgpu::VertexBufferLayout vertex_buffer_layout;
        vertex_buffer_layout.attributeCount = vertex_attributes.size();
        vertex_buffer_layout.attributes = vertex_attributes.data();
        vertex_buffer_layout.arrayStride = buffer.array_stride;
        switch (buffer.step_mode) {
        case Bindings::GPUVertexStepMode::Vertex:
            vertex_buffer_layout.stepMode = wgpu::VertexStepMode::Vertex;
            break;
        case Bindings::GPUVertexStepMode::Instance:
            vertex_buffer_layout.stepMode = wgpu::VertexStepMode::Instance;
            break;
        }
        vertex_buffer_layouts.append(vertex_buffer_layout);
    }

    vertex_state.bufferCount = vertex_buffer_layouts.size();
    vertex_state.buffers = vertex_buffer_layouts.data();
    render_pipeline_descriptor.vertex = vertex_state;

    Optional<wgpu::PrimitiveState> maybe_primitive_state;
    if (auto const& primiate_state_binding = descriptor.primitive; primiate_state_binding.has_value()) {
        wgpu::PrimitiveState primitive_state {};
        switch (primiate_state_binding->topology) {
        case Bindings::GPUPrimitiveTopology::PointList: {
            primitive_state.topology = wgpu::PrimitiveTopology::PointList;
            break;
        }
        case Bindings::GPUPrimitiveTopology::LineList: {
            primitive_state.topology = wgpu::PrimitiveTopology::LineList;
            break;
        }
        case Bindings::GPUPrimitiveTopology::LineStrip: {
            primitive_state.topology = wgpu::PrimitiveTopology::LineStrip;
            break;
        }
        case Bindings::GPUPrimitiveTopology::TriangleList: {
            primitive_state.topology = wgpu::PrimitiveTopology::TriangleList;
            break;
        }
        case Bindings::GPUPrimitiveTopology::TriangleStrip: {
            primitive_state.topology = wgpu::PrimitiveTopology::TriangleStrip;
            break;
        }
        }

        // FIXME: Support this properly so it is undefined for non-strip topologies
        // switch (primiate_state_binding->strip_index_format) {
        // case Bindings::GPUIndexFormat::Uint16: {
        //     primitive_state.stripIndexFormat = wgpu::IndexFormat::Uint16;
        //     break;
        // }
        // case Bindings::GPUIndexFormat::Uint32: {
        //     primitive_state.stripIndexFormat = wgpu::IndexFormat::Uint32;
        //     break;
        // }
        // }

        switch (primiate_state_binding->front_face) {
        case Bindings::GPUFrontFace::Ccw: {
            primitive_state.frontFace = wgpu::FrontFace::CCW;
            break;
        }
        case Bindings::GPUFrontFace::Cw: {
            primitive_state.frontFace = wgpu::FrontFace::CW;
            break;
        }
        }

        switch (primiate_state_binding->cull_mode) {
        case Bindings::GPUCullMode::None: {
            primitive_state.cullMode = wgpu::CullMode::None;
            break;
        }
        case Bindings::GPUCullMode::Front: {
            primitive_state.cullMode = wgpu::CullMode::Front;
            break;
        }
        case Bindings::GPUCullMode::Back: {
            primitive_state.cullMode = wgpu::CullMode::Back;
            break;
        }
        }

        primitive_state.unclippedDepth = primiate_state_binding->unclipped_depth;
        maybe_primitive_state = primitive_state;
    }
    if (maybe_primitive_state.has_value())
        render_pipeline_descriptor.primitive = maybe_primitive_state.value();

    Optional<wgpu::DepthStencilState> maybe_depth_stencil_state;
    if (auto const& depth_stencil_state_binding = descriptor.depth_stencil; depth_stencil_state_binding.has_value()) {
        wgpu::DepthStencilState depth_stencil_state {};
        wgpu::TextureFormat depth_stencil_state_format = wgpu::TextureFormat::Undefined;
        // FIXME: Support remaining texture formats
        switch (depth_stencil_state_binding->format) {
        case Bindings::GPUTextureFormat::Bgra8unorm:
            depth_stencil_state_format = wgpu::TextureFormat::BGRA8Unorm;
            break;
        case Bindings::GPUTextureFormat::Depth24plus:
            depth_stencil_state_format = wgpu::TextureFormat::Depth24Plus;
            break;
        default:
            break;
        }
        depth_stencil_state.format = depth_stencil_state_format;
        if (depth_stencil_state_binding->depth_write_enabled.has_value())
            depth_stencil_state.depthWriteEnabled = depth_stencil_state_binding->depth_write_enabled.value();
        else
            depth_stencil_state.depthWriteEnabled = wgpu::OptionalBool::Undefined;
        wgpu::CompareFunction depth_stencil_state_compare_function = wgpu::CompareFunction::Undefined;
        switch (depth_stencil_state_binding->depth_compare) {
        case Bindings::GPUCompareFunction::Never: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::Never;
            break;
        }
        case Bindings::GPUCompareFunction::Less: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::Less;
            break;
        }
        case Bindings::GPUCompareFunction::Equal: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::Equal;
            break;
        }
        case Bindings::GPUCompareFunction::LessEqual: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::LessEqual;
            break;
        }
        case Bindings::GPUCompareFunction::Greater: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::Greater;
            break;
        }
        case Bindings::GPUCompareFunction::NotEqual: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::NotEqual;
            break;
        }
        case Bindings::GPUCompareFunction::GreaterEqual: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::GreaterEqual;
            break;
        }
        case Bindings::GPUCompareFunction::Always: {
            depth_stencil_state_compare_function = wgpu::CompareFunction::Always;
            break;
        }
        }
        depth_stencil_state.depthCompare = depth_stencil_state_compare_function;
        maybe_depth_stencil_state = depth_stencil_state;
    }
    if (maybe_depth_stencil_state.has_value())
        render_pipeline_descriptor.depthStencil = &maybe_depth_stencil_state.value();

    Optional<wgpu::FragmentState> maybe_fragment_state;
    Vector<wgpu::ColorTargetState> color_target_states;
    if (auto const& fragment_state_binding = descriptor.fragment; fragment_state_binding.has_value()) {
        for (auto const& target : fragment_state_binding->targets) {
            wgpu::ColorTargetState color_target_state {};
            // FIXME: Support remaining texture formats
            switch (target.format) {
            case Bindings::GPUTextureFormat::Bgra8unorm:
                color_target_state.format = wgpu::TextureFormat::BGRA8Unorm;
                break;
            default:
                VERIFY_NOT_REACHED();
            }
            color_target_states.append(color_target_state);
        }

        wgpu::FragmentState fragment_state;
        fragment_state.module = fragment_state_binding->module->native_gpu_shader_module().m_impl->m_shader_module;
        if (fragment_state_binding->entry_point.has_value()) {
            auto entry_point = fragment_state_binding->entry_point.value().bytes_as_string_view();
            fragment_state.entryPoint = wgpu::StringView { entry_point.characters_without_null_termination(), entry_point.length() };
        }
        fragment_state.targetCount = static_cast<uint32_t>(color_target_states.size());
        fragment_state.targets = color_target_states.data();

        maybe_fragment_state = fragment_state;
    }
    if (maybe_fragment_state.has_value())
        render_pipeline_descriptor.fragment = &maybe_fragment_state.value();

    render_pipeline.m_impl->m_render_pipeline = m_device.CreateRenderPipeline(&render_pipeline_descriptor);
    return render_pipeline;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createcommandencoder
NativeGPUCommandEncoder NativeGPUDevice::Impl::create_command_encoder([[maybe_unused]] Optional<GPUCommandEncoderDescriptor> descriptor) const
{
    // FIXME: Implement specification

    auto command_encoder = NativeGPUCommandEncoder::create();
    wgpu::CommandEncoderDescriptor command_encoder_descriptor {};
    command_encoder.m_impl->m_command_encoder = m_device.CreateCommandEncoder(&command_encoder_descriptor);
    return command_encoder;
}

NativeGPUDevice NativeGPUDevice::create()
{
    return NativeGPUDevice(Impl {});
}

// NOTE: wgpu::Device does not have a GetLabel() method exposed

String const& NativeGPUDevice::label() const
{
    return m_impl->m_label;
}

void NativeGPUDevice::set_label(String const& label)
{
    m_impl->m_label = label;
    auto const label_view = label.bytes_as_string_view();
    m_impl->m_device.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

NativeGPUQueue NativeGPUDevice::queue() const
{
    return m_impl->queue();
}

NativeGPUBuffer NativeGPUDevice::create_buffer(GPUBufferDescriptor const& descriptor) const
{
    return m_impl->create_buffer(descriptor);
}

NativeGPUBindGroup NativeGPUDevice::create_bind_group(GPUBindGroupDescriptor const& descriptor) const
{
    return m_impl->create_bind_group(descriptor);
}

NativeGPUShaderModule NativeGPUDevice::create_shader_module(GPUShaderModuleDescriptor const& descriptor) const
{
    return m_impl->create_shader_module(descriptor);
}

NativeGPURenderPipeline NativeGPUDevice::create_render_pipeline(GPURenderPipelineDescriptor const& descriptor) const
{
    return m_impl->create_render_pipeline(descriptor);
}

NativeGPUCommandEncoder NativeGPUDevice::create_command_encoder(Optional<GPUCommandEncoderDescriptor> descriptor) const
{
    return m_impl->create_command_encoder(descriptor);
}

}
