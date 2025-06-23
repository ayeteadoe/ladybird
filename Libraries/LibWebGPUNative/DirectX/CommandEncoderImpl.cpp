/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebGPUNative/DirectX/CommandEncoderImpl.h>
#include <LibWebGPUNative/DirectX/DeviceImpl.h>
#include <LibWebGPUNative/DirectX/Error.h>
#include <LibWebGPUNative/DirectX/RenderPassEncoderImpl.h>
#include <LibWebGPUNative/DirectX/TextureViewImpl.h>

namespace WebGPUNative {

CommandEncoder::Impl::Impl(Device const& gpu_device)
    : m_device(gpu_device.m_impl->device())
    , m_command_allocator(gpu_device.m_impl->command_allocator())
{
}

ErrorOr<void> CommandEncoder::Impl::initialize()
{
    if (HRESULT const result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command_list)); FAILED(result))
        return make_error(result, "Unable to create command list");
    return {};
}

ErrorOr<void> CommandEncoder::Impl::begin_render_pass(RenderPassEncoder const& render_pass_encoder)
{
    // FIXME: Don't hardcode these settings
    RenderPassDescriptor const& render_pass_descriptor = render_pass_encoder.render_pass_descriptor();
    for (auto const& color_attachment : render_pass_descriptor.color_attachments) {
        auto texture_view = color_attachment.view;
        if (color_attachment.clear_value.has_value()) {
            auto const& [r, g, b, a] = color_attachment.clear_value.value();
            float const clear_color[] = { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a) };
            m_command_list->ClearRenderTargetView(texture_view->m_impl->texture_view_handle(), clear_color, 0, nullptr);
        }
        auto const view_size = texture_view->m_impl->size();
        D3D12_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(view_size.width());
        viewport.Height = static_cast<float>(view_size.height());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_command_list->RSSetViewports(1, &viewport);

        D3D12_RECT scissor_rect = {};
        scissor_rect.left = 0;
        scissor_rect.top = 0;
        scissor_rect.right = static_cast<LONG>(view_size.width());
        scissor_rect.bottom = static_cast<LONG>(view_size.height());

        m_command_list->RSSetScissorRects(1, &scissor_rect);
    }
    return {};
}

ErrorOr<void> CommandEncoder::Impl::finish()
{
    if (HRESULT const result = m_command_list->Close(); FAILED(result))
        return make_error(result, "Unable to finish command list");
    Device::Impl::log_debug_info(m_device);
    return {};
}

}
