#pragma once


#include <defines.h>

struct EventContext {
    // 16 bytes
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];

        char c[16];
    } data;
};

// should return true if handled
using EventCallback = b8 (*)(u16 code, void* sender, void* listener, EventContext context);

b8 event_initialize();
void event_shutdown();

// returns true if successful registration, false if failed 
MGO_API b8 event_register(u16 code, void* listener, EventCallback callback);

// returns true if successful unregistration, false if failed
MGO_API b8 event_unregister(u16 code, void* listener, EventCallback callback);

// returns true if event was handled, false doesnt mean failed necesarily
MGO_API b8 event_fire(u16 code, void* sender, EventContext context);


enum system_event_code {
    // Shuts the application down on the next frame.
    EVENT_CODE_APPLICATION_QUIT = 1,

    // Keyboard key pressed.
    /* Context usage:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_PRESSED = 2,

    // Keyboard key released.
    /* Context usage:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_RELEASED = 3,

    // Mouse button pressed.
    /* Context usage:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_PRESSED = 4,

    // Mouse button released.
    /* Context usage:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_RELEASED = 5,

    // Mouse moved.
    /* Context usage:
     * u16 x = data.data.u16[0];
     * u16 y = data.data.u16[1];
     */
    EVENT_CODE_MOUSE_MOVED = 6,

    // Mouse moved.
    /* Context usage:
     * u8 z_delta = data.data.u8[0];
     */
    EVENT_CODE_MOUSE_WHEEL = 7,

    // Resized/resolution changed from the OS.
    /* Context usage:
     * u16 width = data.data.u16[0];
     * u16 height = data.data.u16[1];
     */
    EVENT_CODE_RESIZED = 8,

    
    MAX_EVENT_CODES = 255
};