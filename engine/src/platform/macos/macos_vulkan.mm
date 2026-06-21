#include "defines.h"

#if MGO_PLATFORM_APPLE

#include "renderer/vulkan/vulkan_platform.h"

#include <containers/darray.h>
#include <vulkan/vulkan_metal.h>

void get_required_extensions(darray<const char *> &extensions) {
  extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
}

VkSurfaceKHR create_surface(VkInstance instance, void *nativeWindow) {
  // NSWindow* conversion here
  // vkCreateMetalSurfaceEXT(...)
  return nullptr;
}

#endif
