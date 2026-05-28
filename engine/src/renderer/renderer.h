#pragma once

#include "renderer_api.h"
#include "renderer_types.h"

struct static_mesh_data;

class Renderer {
    public:
        b8 init(const char* app_name);
        void shutdown();
        void on_resized(u16 width, u16 height);
        void draw_frame(RenderPacket* packet);
};