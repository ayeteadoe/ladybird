/*
 * Copyright (c) 2025, ayeteadoe <ayeteadoe@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Vector.h>
#include <LibWebGPUNative/Vulkan/Error.h>
#include <LibWebGPUNative/Vulkan/InstanceImpl.h>

#ifdef WEBGPUNATIVE_DEBUG

namespace {

constexpr char debug_message_prefix[] = "WebGPUNative [Vulkan]";

VkDebugUtilsMessengerEXT debug_messenger { VK_NULL_HANDLE };

VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void*)
{
    dbgln("{}: {}", debug_message_prefix, pCallbackData->pMessage);
    return VK_FALSE;
}

VkResult create_debug_utils_messenger(
    VkInstance instance,
    VkDebugUtilsMessengerCreateInfoEXT const* create_info,
    VkAllocationCallbacks const* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, create_info, allocator, debug_messenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger(VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    VkAllocationCallbacks const* allocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(instance, debug_messenger, allocator);
    }
}

}

#endif

namespace WebGPUNative {

Instance::Impl::~Impl()
{
    if (m_instance) {
#ifdef WEBGPUNATIVE_DEBUG
        destroy_debug_utils_messenger(m_instance, debug_messenger, nullptr);
#endif
        vkDestroyInstance(m_instance, nullptr);
    }
}

ErrorOr<void> Instance::Impl::initialize()
{
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Ladybird WebGPU Native";
    application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    application_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    application_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;

#ifdef WEBGPUNATIVE_DEBUG
    Vector const enabled_extensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();
#else
    instance_create_info.enabledExtensionCount = 0;
    instance_create_info.ppEnabledExtensionNames = nullptr;
#endif
    instance_create_info.pNext = nullptr;

#ifdef WEBGPUNATIVE_DEBUG
    const Vector validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    instance_create_info.ppEnabledLayerNames = validation_layers.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = debug_callback;
    debug_messenger_create_info.pUserData = nullptr;

    instance_create_info.pNext = &debug_messenger_create_info;
#endif

    if (VkResult const create_instance_result = vkCreateInstance(&instance_create_info, nullptr, &m_instance); create_instance_result != VK_SUCCESS)
        return make_error(create_instance_result, "Unable to create instance");

#ifdef WEBGPUNATIVE_DEBUG
    if (create_debug_utils_messenger(m_instance, &debug_messenger_create_info, nullptr, &debug_messenger) != VK_SUCCESS)
        dbgln("{}: Failed to create debug messenger", debug_message_prefix);
#endif

    return {};
}

}
