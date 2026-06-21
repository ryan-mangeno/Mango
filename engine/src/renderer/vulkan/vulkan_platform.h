#pragma once

#include <vulkan/vulkan.h>

#include <containers/darray.h>

// Pushes required extensions to input array
//
// macOS -> VK_EXT_metal_surface
// Windows -> VK_KHR_win32_surface
void get_required_extensions(darray<const char *> &extensions);

// Creates the VkSurfaceKHR for the current OS window.
//
// The window pointer is platform-specific:
// macOS  -> NSWindow*
// Windows -> HWND
//
VkSurfaceKHR create_surface(VkInstance instance, void *native_window);
