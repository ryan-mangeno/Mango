#include "renderer.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "core/mgmemory.h"

static RendererAPI* s_renderer_api = nullptr;
static u64 s_frame_number = 0;

b8 renderer_init(const char* app_name) {
    
    if (!(s_renderer_api = RendererBackend::create())) {
        MGO_FATAL("Failed to create Renderer Backend. Shutting down.");
        return FALSE;
    }

    if (!s_renderer_api->init(app_name)) {
        MGO_FATAL("Failed to initialize Renderer Backend. Shutting down.");
        return FALSE;
    }

    s_frame_number = 0;

    return TRUE;
}

void renderer_shutdown() {
    s_renderer_api->shutdown();
    mg_delete(s_renderer_api, MEMORY_TAG_RENDERER);
    s_renderer_api = nullptr;

}

void renderer_on_resized(u16 width, u16 height) {

}

b8 renderer_begin_frame(f32 delta_time) {
    return s_renderer_api->begin_frame(delta_time);
}

b8 renderer_end_frame(f32 delta_time) {
    b8 result = s_renderer_api->end_frame(delta_time);
    s_frame_number++;
    return result;
}

b8 renderer_draw_frame(RenderPacket* packet) {
    if (s_renderer_api->begin_frame(packet->delta_time)) {

        b8 result = s_renderer_api->end_frame(packet->delta_time);

        if (!result) {
            MGO_ERROR("RendererAPI end_frame failed. Application shutting down...");
            return FALSE;
        }
    }

    return TRUE;
}
