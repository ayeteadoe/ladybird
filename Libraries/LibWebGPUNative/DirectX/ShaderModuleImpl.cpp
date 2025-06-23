/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWGSL/Compiler.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>
#include <LibWebGPUNative/DirectX/ShaderModuleImpl.h>
#include <d3dcompiler.h>

namespace WebGPUNative {

ShaderModule::Impl::Impl(Device const& gpu_device, ShaderModuleDescriptor const& gpu_shader_module_descriptor)
    : m_device(gpu_device.m_impl->device())
    , m_shader_module_descriptor(gpu_shader_module_descriptor)
{
}

ErrorOr<void> ShaderModule::Impl::initialize()
{
    auto create_shader_blob = [](ByteString const& shader_source, char const* target) -> ErrorOr<ComPtr<ID3DBlob>> {
        ComPtr<ID3DBlob> shader_blob;
        ComPtr<ID3DBlob> error_blob;
        if (HRESULT const result = D3DCompile(shader_source.characters(), shader_source.length(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, 0, 0, shader_blob.GetAddressOf(), error_blob.GetAddressOf()); FAILED(result)) {
            auto const error_string = ByteString(static_cast<char const*>(error_blob->GetBufferPointer()), error_blob->GetBufferSize());
            dbgln(error_string);
            return make_error(result, "Unable to compile HLSL vertex source");
        }
        return shader_blob;
    };

    WGSL::Compiler compiler(m_shader_module_descriptor.code);
    auto const hlsl_shader_sources = TRY(compiler.emit_hlsl());
    for (auto const& shader_source : hlsl_shader_sources) {
        TRY(shader_source.visit(
            [this, &create_shader_blob](WGSL::Compiler::VertexShader const& vertex_shader) -> ErrorOr<void> {
                VertexShader shader;
                shader.entry_point = vertex_shader.entry_point;
                auto const vertex_shader_source = vertex_shader.source.to_byte_string();
                shader.blob = TRY(create_shader_blob(vertex_shader_source, "vs_5_0"));
                m_shader_targets.append(move(shader));
                return {};
            },
            [this, &create_shader_blob](WGSL::Compiler::FragmentShader const& fragment_shader) -> ErrorOr<void> {
                FragmentShader shader;
                shader.entry_point = fragment_shader.entry_point;
                auto const fragment_shader_source = fragment_shader.source.to_byte_string();
                shader.blob = TRY(create_shader_blob(fragment_shader_source, "ps_5_0"));
                m_shader_targets.append(move(shader));
                return {};
            }));
    }
    return {};
}

RawPtr<VertexShader const> ShaderModule::Impl::vertex(StringView const entry_point) const
{
    RawPtr<VertexShader const> shader = { nullptr };
    for (auto const& shader_target : m_shader_targets) {
        shader_target.visit(
            [](auto const&) {
            },
            [&entry_point, &shader](VertexShader const& vertex_shader) {
                if (vertex_shader.entry_point == entry_point)
                    shader = &vertex_shader;
            });
    }
    return shader;
}

RawPtr<FragmentShader const> ShaderModule::Impl::fragment(StringView const entry_point) const
{
    RawPtr<FragmentShader const> shader = { nullptr };
    for (auto const& shader_target : m_shader_targets) {
        shader_target.visit(
            [](auto const&) {
            },
            [&entry_point, &shader](FragmentShader const& fragment_shader) {
                if (fragment_shader.entry_point == entry_point)
                    shader = &fragment_shader;
            });
    }
    return shader;
}

}
