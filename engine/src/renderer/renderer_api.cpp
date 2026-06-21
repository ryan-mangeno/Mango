#include "renderer_api.h"

// TODO: in future will add more support for other apis
// focusing on vulkan for now
#ifdef MGO_PLATFORM_APPLE
GraphicsAPI RendererAPI::s_api = GraphicsAPI::Vulkan;
#elif MGO_PLATFORM_WINDOWS
GraphicsAPI RendererAPI::s_api = GraphicsAPI::Vulkan;
#endif
