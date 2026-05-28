#include "vulkan_api.h"


b8 VulkanAPI::init(const char* app_name) {
    return TRUE;
}

void VulkanAPI::shutdown() {

}

void VulkanAPI::resized(u16 width, u16 height) {

}

b8 VulkanAPI::begin_frame(f32 delta_time) {
    return TRUE;
}

b8 VulkanAPI::end_frame(f32 delta_time) {
    return TRUE;
}
