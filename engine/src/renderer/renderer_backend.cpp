#include "renderer_backend.h"

#include <core/logger.h>

RendererAPI* RendererBackend::create() {
    
    RendererAPI* out_api = nullptr;

    switch (RendererAPI::get_api()) {
        case GraphicsAPI::Vulkan:
            // out_api = ...
            break;

        default: 
            MGO_ERROR("Invalid Graphics API");
            break;
    }

    return out_api;

}
