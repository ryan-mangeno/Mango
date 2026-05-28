#pragma once

#include "renderer_types.h"
#include "renderer_api.h"

class RendererBackend {
    public:
        static RendererAPI* create();
};