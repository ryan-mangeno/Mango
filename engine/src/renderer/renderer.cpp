#include "renderer.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "core/mgmemory.h"

static RendererAPI* s_renderer_api = nullptr;
static u64 s_frame_number = 0;

namespace Renderer {

    b8 init(const char* app_name) {
        
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

    void shutdown() {
        s_renderer_api->shutdown();
        mg_free(s_renderer_api, sizeof(RendererAPI), MEMORY_TAG_RENDERER);
    }

    void on_resized(u16 width, u16 height) {

    }

    b8 begin_frame(f32 delta_time) {
        return s_renderer_api->begin_frame(delta_time);
    }

    b8 end_frame(f32 delta_time) {
        b8 result = s_renderer_api->end_frame(delta_time);
        s_frame_number++;
        return result;
    }

    b8 draw_frame(RenderPacket* packet) {
        if (s_renderer_api->begin_frame(packet->delta_time)) {

            b8 result = s_renderer_api->end_frame(packet->delta_time);

            if (!result) {
                MGO_ERROR("RendererAPI end_frame failed. Application shutting down...");
                return FALSE;
            }
        }

        return TRUE;
    }

}