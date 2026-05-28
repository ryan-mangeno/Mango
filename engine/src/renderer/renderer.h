#pragma once

#include "defines.h"

struct StaticMeshData;
struct RenderPacket;

namespace Renderer {
    b8 init(const char* app_name);
    void shutdown();
    void on_resized(u16 width, u16 height);
    b8 begin_frame(f32 delta_time);
    b8 end_frame(f32 delta_time);
    b8 draw_frame(RenderPacket* packet);
}