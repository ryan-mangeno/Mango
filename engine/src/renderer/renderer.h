#pragma once


struct StaticMeshData;
struct RenderPacket;

class Renderer {
    public:
        static b8 init(const char* app_name);
        static void shutdown();
        static void on_resized(u16 width, u16 height);
        static void draw_frame(RenderPacket* packet);
};