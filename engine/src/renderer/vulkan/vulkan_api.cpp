#include "vulkan_api.h"
#include "vulkan_types.h"
#include "core/logger.h"
#include "core/mgstring.h"

#include "containers/darray.h"

#include "platform/platform.h"

// include the beta header only on Mac for the portability extension name string
#if defined(__APPLE__)
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#endif

static VulkanContext s_context;

b8 VulkanAPI::init(const char* app_name) {
    s_context.allocator = 0;

    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Mango Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = 0;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    // hardcoding for now
#if defined(MGO_PLATFORM_APPLE)
    const char* apple_extensions[] = {
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    };
    create_info.enabledExtensionCount = 1;
    create_info.ppEnabledExtensionNames = apple_extensions;
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkResult result = vkCreateInstance(&create_info, s_context.allocator, &s_context.instance);
    if(result != VK_SUCCESS) {
        MGO_ERROR("vkCreateInstance failed with result: %d", (i32)result);
        return FALSE;
    }

    MGO_INFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void VulkanAPI::shutdown() {
    if (s_context.instance) {
        vkDestroyInstance(s_context.instance, s_context.allocator);
        s_context.instance = nullptr;
    }
}

void VulkanAPI::resized(u16 width, u16 height) {

}

b8 VulkanAPI::begin_frame(f32 delta_time) {
    return TRUE;
}

b8 VulkanAPI::end_frame(f32 delta_time) {
    return TRUE;
}