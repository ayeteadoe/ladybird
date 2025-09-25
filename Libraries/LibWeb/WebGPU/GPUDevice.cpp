/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebGPU/GPU.h>
#include <LibWeb/WebGPU/GPUBuffer.h>
#include <LibWeb/WebGPU/GPUCommandEncoder.h>
#include <LibWeb/WebGPU/GPUDevice.h>
#include <LibWeb/WebGPU/GPUShaderModule.h>
#include <LibWeb/WebGPU/GPUTexture.h>
#include <LibWeb/WebIDL/Promise.h>

#include <webgpu/webgpu_cpp.h>

namespace Web::WebGPU {

GC_DEFINE_ALLOCATOR(GPUDevice);

struct GPUDevice::Impl {
    wgpu::Device device { nullptr };
    String label;
    GC::Ref<GPU> instance;
    GC::Ref<GPUQueue> queue;
};

GPUDevice::GPUDevice(JS::Realm& realm, Impl impl)
    : EventTarget(realm)
    , m_impl(make<Impl>(move(impl)))
{
}

GPUDevice::~GPUDevice() = default;

JS::ThrowCompletionOr<GC::Ref<GPUDevice>> GPUDevice::create(JS::Realm& realm, GPU& instance, wgpu::Device device, String const& label)
{
    auto queue = device.GetQueue();
    return realm.create<GPUDevice>(realm, Impl { .device = move(device), .label = label, .instance = instance, .queue = TRY(GPUQueue::create(realm, instance, move(queue))) });
}

void GPUDevice::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(GPUDevice);
    EventTarget::initialize(realm);
}

void GPUDevice::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_impl->instance);
    visitor.visit(m_impl->queue);
}

wgpu::Device GPUDevice::wgpu() const
{
    return m_impl->device;
}

// NOTE: wgpu::Device does not have a GetLabel() method exposed

String const& GPUDevice::label() const
{
    return m_impl->label;
}

void GPUDevice::set_label(String const& label)
{
    m_impl->label = label;
    auto label_view = label.bytes_as_string_view();
    m_impl->device.SetLabel(wgpu::StringView { label_view.characters_without_null_termination(), label_view.length() });
}

GC::Ref<GPU> GPUDevice::instance() const
{
    return m_impl->instance;
}

// FIXME: Support device lost promise
GC::Ref<WebIDL::Promise> GPUDevice::lost() const
{
    auto& realm = this->realm();
    GC::Ref promise = WebIDL::create_promise(realm);
    // m_impl->device.GetLostFuture()
    return promise;
}

GC::Ref<GPUQueue> GPUDevice::queue() const
{
    return m_impl->queue;
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createbuffer
GC::Ref<GPUBuffer> GPUDevice::create_buffer(GPUBufferDescriptor const& options) const
{
    wgpu::BufferDescriptor buffer_descriptor {};
    buffer_descriptor.size = options.size;
    buffer_descriptor.mappedAtCreation = options.mapped_at_creation;
    // FIXME: Support all buffer usage flags
    wgpu::BufferUsage buffer_usage {};
    auto const map_read = static_cast<bool>(options.usage & static_cast<u32>(wgpu::BufferUsage::MapRead));
    auto const map_write = static_cast<bool>(options.usage & static_cast<u32>(wgpu::BufferUsage::MapWrite));
    auto const copy_src = static_cast<bool>(options.usage & static_cast<u32>(wgpu::BufferUsage::CopySrc));
    auto const copy_dest = static_cast<bool>(options.usage & static_cast<u32>(wgpu::BufferUsage::CopyDst));
    auto const vertex = static_cast<bool>(options.usage & static_cast<u32>(wgpu::BufferUsage::Vertex));
    if (map_read) {
        buffer_usage |= wgpu::BufferUsage::MapRead;
    }
    if (map_write) {
        buffer_usage |= wgpu::BufferUsage::MapWrite;
    }
    if (copy_src) {
        buffer_usage |= wgpu::BufferUsage::CopySrc;
    }
    if (copy_dest) {
        buffer_usage |= wgpu::BufferUsage::CopyDst;
    }
    if (vertex) {
        buffer_usage |= wgpu::BufferUsage::Vertex;
    }
    buffer_descriptor.usage = buffer_usage;

    // Content timeline steps:
    // 1. Let b be ! create a new WebGPU object(this, GPUBuffer, descriptor).
    // 2. Set b.size to descriptor.size.
    // 3. Set b.usage to descriptor.usage.
    // 4. FIXME: If descriptor.mappedAtCreation is true:
    //      1. If descriptor.size is not a multiple of 4, throw a RangeError.
    //      2. Set b.[[mapping]] to ? initialize an active buffer mapping with mode WRITE and range [0, descriptor.size].
    if (buffer_descriptor.mappedAtCreation) {
    }

    // FIXME: Issue the initialization steps on the Device timeline of this.

    wgpu::Buffer native_buffer = m_impl->device.CreateBuffer(&buffer_descriptor);

    auto& realm = this->realm();
    return MUST(GPUBuffer::create(realm, m_impl->instance, move(native_buffer)));
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createtexture
// FIXME: Spec comments
GC::Ref<GPUTexture> GPUDevice::create_texture(GPUTextureDescriptor const& options) const
{
    auto to_view_format = [](Bindings::GPUTextureFormat format) -> Optional<wgpu::TextureFormat> {
        // FIXME: Support remaining texture formats
        switch (format) {
        case Bindings::GPUTextureFormat::Bgra8unorm:
            return wgpu::TextureFormat::BGRA8Unorm;
        default:
            return {};
        }
    };
    Vector<wgpu::TextureFormat> texture_formats;
    for (auto const& view_format : options.view_formats) {
        auto const texture_format = to_view_format(view_format);
        if (texture_format.has_value()) {
            texture_formats.append(texture_format.value());
        }
    }

    wgpu::Extent3D extent { .width = options.size.width, .height = options.size.height, .depthOrArrayLayers = options.size.depth_or_array_layers };
    wgpu::TextureDescriptor texture_descriptor { .size = extent, .mipLevelCount = options.mip_level_count, .sampleCount = options.sample_count, .viewFormatCount = texture_formats.size(), .viewFormats = texture_formats.data() };
    auto const wgpu_format = to_view_format(options.format);
    if (wgpu_format.has_value()) {
        texture_descriptor.format = wgpu_format.value();
    }
    switch (options.dimension) {
    case Bindings::GPUTextureDimension::_1d: {
        texture_descriptor.dimension = wgpu::TextureDimension::e1D;
        break;
    }
    case Bindings::GPUTextureDimension::_2d: {
        texture_descriptor.dimension = wgpu::TextureDimension::e2D;
        break;
    }
    case Bindings::GPUTextureDimension::_3d: {
        texture_descriptor.dimension = wgpu::TextureDimension::e3D;
        break;
    }
    default:
        break;
    }
    wgpu::TextureUsage texture_usage = wgpu::TextureUsage::None;
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::CopySrc)) {
        texture_usage |= wgpu::TextureUsage::CopySrc;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::CopyDst)) {
        texture_usage |= wgpu::TextureUsage::CopyDst;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::TextureBinding)) {
        texture_usage |= wgpu::TextureUsage::TextureBinding;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::StorageBinding)) {
        texture_usage |= wgpu::TextureUsage::StorageBinding;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::RenderAttachment)) {
        texture_usage |= wgpu::TextureUsage::RenderAttachment;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::TransientAttachment)) {
        texture_usage |= wgpu::TextureUsage::TransientAttachment;
    }
    if (options.usage & static_cast<u32>(wgpu::TextureUsage::StorageAttachment)) {
        texture_usage |= wgpu::TextureUsage::StorageAttachment;
    }
    texture_descriptor.usage = texture_usage;
    wgpu::Texture native_texture = m_impl->device.CreateTexture(&texture_descriptor);
    auto& realm = this->realm();
    return MUST(GPUTexture::create(realm, move(native_texture)));
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createcommandencoder
// FIXME: Spec comments
GC::Ref<GPUCommandEncoder> GPUDevice::create_command_encoder(GPUCommandEncoderDescriptor const& options) const
{
    wgpu::CommandEncoderDescriptor command_encoder_descriptor_options = options.to_wgpu();
    wgpu::CommandEncoder native_command_encoder = m_impl->device.CreateCommandEncoder(&command_encoder_descriptor_options);
    auto& realm = this->realm();
    return MUST(GPUCommandEncoder::create(realm, move(native_command_encoder)));
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createshadermodule
GC::Ref<GPUShaderModule> GPUDevice::create_shader_module(GPUShaderModuleDescriptor const& options) const
{
    wgpu::ShaderSourceWGSL wgsl_source;
    auto code_view = options.code.bytes_as_string_view();
    auto code_view_bytes = code_view.bytes();
    wgsl_source.code = wgpu::StringView { bit_cast<char const*>(code_view_bytes.data()), code_view_bytes.size() };

    wgpu::ShaderModuleDescriptor shader_module_descriptor_options { .nextInChain = &wgsl_source };

    wgpu::ShaderModule native_shader_module = m_impl->device.CreateShaderModule(&shader_module_descriptor_options);

    auto& realm = this->realm();
    return MUST(GPUShaderModule::create(realm, move(native_shader_module)));
}

// https://www.w3.org/TR/webgpu/#dom-gpudevice-createrenderpipeline
// FIXME: Spec comments
GC::Ref<GPURenderPipeline> GPUDevice::create_render_pipeline(GPURenderPipelineDescriptor const& options) const
{
    wgpu::VertexState vertex_state {};
    vertex_state.module = options.vertex.module->as_wgpu();
    if (options.vertex.entry_point.has_value()) {
        auto entry_point = options.vertex.entry_point.value().bytes_as_string_view();
        vertex_state.entryPoint = wgpu::StringView { entry_point.characters_without_null_termination(), entry_point.length() };
    }
    // FIXME: Set remaining VertexState fields

    wgpu::PrimitiveState primitive_state {};
    switch (options.primitive.topology) {
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
    // switch (options.primitive.strip_index_format) {
    // case Bindings::GPUIndexFormat::Uint16: {
    //     primitive_state.stripIndexFormat = wgpu::IndexFormat::Uint16;
    //     break;
    // }
    // case Bindings::GPUIndexFormat::Uint32: {
    //     primitive_state.stripIndexFormat = wgpu::IndexFormat::Uint32;
    //     break;
    // }
    // }

    switch (options.primitive.front_face) {
    case Bindings::GPUFrontFace::Ccw: {
        primitive_state.frontFace = wgpu::FrontFace::CCW;
        break;
    }
    case Bindings::GPUFrontFace::Cw: {
        primitive_state.frontFace = wgpu::FrontFace::CW;
        break;
    }
    }

    switch (options.primitive.cull_mode) {
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

    primitive_state.unclippedDepth = options.primitive.unclipped_depth;

    Vector<wgpu::ColorTargetState> color_target_states;
    for (auto const& target : options.fragment.targets) {
        wgpu::ColorTargetState color_target_state {};
        wgpu::TextureFormat format = wgpu::TextureFormat::Undefined;
        // FIXME: Support remaining texture formats
        switch (target.format) {
        case Bindings::GPUTextureFormat::Bgra8unorm:
            format = wgpu::TextureFormat::BGRA8Unorm;
            break;
        default:
            break;
        }
        color_target_state.format = format;
        // FIXME: Support blend
        // FIXME: Support writeMask

        color_target_states.append(color_target_state);
    }

    wgpu::FragmentState fragment_state;
    fragment_state.module = options.fragment.module->as_wgpu();
    if (options.fragment.entry_point.has_value()) {
        auto entry_point = options.fragment.entry_point.value().bytes_as_string_view();
        fragment_state.entryPoint = wgpu::StringView { entry_point.characters_without_null_termination(), entry_point.length() };
    }
    fragment_state.targetCount = static_cast<uint32_t>(color_target_states.size());
    fragment_state.targets = color_target_states.data();

    wgpu::RenderPipelineDescriptor render_pipeline_descriptor {};
    // FIXME: Support layout
    render_pipeline_descriptor.vertex = vertex_state;
    render_pipeline_descriptor.primitive = primitive_state;
    // FIXME: Support depthStencil
    // FIXME: Support multisample
    render_pipeline_descriptor.fragment = &fragment_state;

    wgpu::RenderPipeline native_render_pipeline = m_impl->device.CreateRenderPipeline(&render_pipeline_descriptor);

    auto& realm = this->realm();
    return MUST(GPURenderPipeline::create(realm, move(native_render_pipeline)));
}

}
