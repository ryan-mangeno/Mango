#pragma once

#include "defines.h"

struct StaticMeshData;
struct RenderPacket;

b8 renderer_init(const char* app_name);
void renderer_shutdown();
void renderer_on_resized(u16 width, u16 height);
b8 renderer_begin_frame(f32 delta_time);
b8 renderer_end_frame(f32 delta_time);
b8 renderer_draw_frame(RenderPacket* packet);