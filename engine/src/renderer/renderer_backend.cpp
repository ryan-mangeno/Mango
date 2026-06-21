#include "renderer_backend.h"
#include "core/mgmemory.h"

#include "vulkan/vulkan_api.h"

RendererAPI *RendererBackend::create() {

  RendererAPI *out_api = nullptr;

  switch (RendererAPI::get_api()) {
  case GraphicsAPI::Vulkan:
    out_api = mg_new<VulkanAPI>(MEMORY_TAG_RENDERER);
    break;

  case GraphicsAPI::DirectX:
  case GraphicsAPI::OpenGL:
  case GraphicsAPI::None:
  default:
    break;
  }

  return out_api;
}
