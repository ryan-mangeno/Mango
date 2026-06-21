#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

struct VulkanContext {
  VkInstance instance;
  VkAllocationCallbacks *allocator;
};
