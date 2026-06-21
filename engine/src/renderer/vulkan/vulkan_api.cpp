#include "vulkan_api.h"
#include "core/logger.h"
#include "core/mgstring.h"
#include "vulkan_types.h"

#include "containers/darray.h"

#include "platform/platform.h"

// include the beta header only on Mac for the portability extension name string
#if defined(__APPLE__)
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#endif

static VulkanContext s_context;

b8 VulkanAPI::init(const char *app_name) {
  s_context.allocator = 0; // TODO: use custom allocator

  VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  app_info.apiVersion = VK_API_VERSION_1_2;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Mango Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  create_info.pApplicationInfo = &app_info;

  darray<const char *> required_extensions;
  _get_required_extensions(required_extensions);

#if defined(_DEBUG)
  required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  MGO_DEBUG("Required Extensions:");
  u64 len = required_extensions.size();
  for (u64 i = 0; i < len; ++i) {
    MGO_DEBUG(required_extensions[i]);
  }
#endif

  create_info.enabledExtensionCount = required_extensions.size();
  create_info.ppEnabledExtensionNames = required_extensions.data();
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledLayerNames = 0;

  VkResult result =
      vkCreateInstance(&create_info, s_context.allocator, &s_context.instance);
  if (result != VK_SUCCESS) {
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

void VulkanAPI::resized(u16 width, u16 height) {}

b8 VulkanAPI::begin_frame(f32 delta_time) { return TRUE; }

b8 VulkanAPI::end_frame(f32 delta_time) { return TRUE; }

void VulkanAPI::_get_required_extensions(darray<const char *> &extensions) {
  extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(MGO_PLATFORM_WINDOWS)

  extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#elif defined(MGO_PLATFORM_APPLE)

  // extensions.push_back(
  // VK_EXT_METAL_SURFACE_EXTENSION_NAME);

  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

#elif defined(MGO_PLATFORM_LINUX)

  extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);

#endif
}
