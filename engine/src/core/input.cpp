#include "input.h"
#include "event.h"
#include "mgmemory.h"
#include "logger.h"

/*
the users actualy doesnt need to know about the implementations of keyboard, mouse, input state
slightly more c style here, and focuses on internal linktage. I am only exposing endpoints to poll input state, which there is really only one of
however, there can be different input systems, keyboard vs gamepad, vs controller, etc, but down the line I would prefer a dependency injection
approach here in order to avoid alot of polymorphic calls with vtable as input will likely be polled alot per game update pass numerous times in the engine/game
*/

struct KeyboardState {
    u64 bits[4]; // bitmask for keyboard state
};

struct MouseState {
    i16 x;
    i16 y;
    u8 bits; // bitmask for mouse state
};

struct InputState {
    KeyboardState keyboard_current;
    KeyboardState keyboard_previous;
    MouseState    mouse_current;
    MouseState    mouse_previous;
};

static b8 s_initialized = FALSE;
static InputState s_state = {};

bool input_initialize() {
    // state is static so is zero initialized by default but is good for clarity
    mg_zero_memory(&s_state, sizeof(InputState)); 
    s_initialized = TRUE;
    MGO_INFO("Input subsystem intialized");

    // may want to handle other input initialization routines for certain systems

    return TRUE;
}

void input_shutdown() {
    // TODO: shutdown routines from different kinds of input systems
    // gamepad, others, etc
    s_initialized = FALSE;
}

void input_update(f64 delta_time) noexcept {
    if (!s_initialized) return;

    mg_copy_memory(&s_state.keyboard_previous, &s_state.keyboard_current, sizeof(KeyboardState));
    mg_copy_memory(&s_state.mouse_previous, &s_state.mouse_current, sizeof(MouseState));
}

void input_process_key(keys key, b8 pressed) noexcept {
    u32 block = key >> 6; 
    u32 idx   = key & 63; 

    b8 is_down = ( s_state.keyboard_current.bits[block] & (1ULL << idx) ) != 0;

    // we only change state our button state and logged keyboard state are different
    if (is_down != pressed) {
        if (pressed) {
            s_state.keyboard_current.bits[block] |=  (1ULL << idx);
        } else {
            s_state.keyboard_current.bits[block] &= ~(1ULL << idx);
        }

        EventContext ctx;
        ctx.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, ctx);
    }
}

void input_process_button(buttons button, b8 pressed) noexcept {
    u32 mask = (1ULL << button);
    b8 is_down = ( s_state.mouse_current.bits & (mask) ) != 0;

    if (is_down != pressed) {
        if(pressed) {
            s_state.mouse_current.bits |= mask;
        } else {
            s_state.mouse_current.bits &= ~mask;
        }

        EventContext ctx;
        ctx.data.u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, ctx);
    }
}

void input_process_mouse_move(i16 x, i16 y) noexcept {
    if (s_state.mouse_current.x != x || s_state.mouse_current.y != y) {
        // NOTE: enable if debugging
        // MGO_DEBUG("Mouse move ...

        s_state.mouse_current.x = x;
        s_state.mouse_current.x = y;

        EventContext ctx;
        ctx.data.u16[0] = x;
        ctx.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, ctx);
    }
}

void input_process_mouse_wheel(i8 z_delta) noexcept {
    // NOTE: no internal state for mouse wheel

    EventContext ctx;
    ctx.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, ctx);
}

// keyboard input
MGO_API b8 input_is_key_down(keys key) noexcept {
    u32 block = key >> 6; // which u64 block does it live in
    u32 idx   = key & 63; // which bit wihtin that u64? (0-63)

    return (s_state.keyboard_current.bits[block] & (1ULL << idx)) != 0;
}

MGO_API b8 input_is_key_up(keys key) noexcept {
    u32 block = key >> 6; 
    u32 idx   = key & 63; 

    return (s_state.keyboard_current.bits[block] & (1ULL << idx)) == 0;
}

MGO_API b8 input_was_key_down(keys key) noexcept {
    u32 block = key >> 6; 
    u32 idx   = key & 63; 

    return (s_state.keyboard_previous.bits[block] & (1ULL << idx)) != 0;
}

MGO_API b8 input_was_key_up(keys key) noexcept {
    u32 block = key >> 6; 
    u32 idx   = key & 63; 

    return (s_state.keyboard_previous.bits[block] & (1ULL << idx)) == 0;
}

// mouse input
MGO_API b8 input_is_button_down(buttons button) noexcept {
    return (s_state.mouse_current.bits & (1 << button)) != 0;
}

MGO_API b8 input_is_button_up(buttons button) noexcept {
    return (s_state.mouse_current.bits & (1 << button)) == 0;
}   

MGO_API b8 input_was_button_down(buttons button) noexcept {
    return (s_state.mouse_previous.bits & (1 << button)) != 0;
}

MGO_API b8 input_was_button_up(buttons button) noexcept {
    return (s_state.mouse_previous.bits & (1 << button)) == 0;
}

MGO_API void input_get_mouse_position(i32& x, i32& y) noexcept {
    x = s_state.mouse_current.x;
    y = s_state.mouse_current.y;
}

MGO_API void input_get_previous_mouse_position(i32& x, i32& y) noexcept {
    x = s_state.mouse_previous.x;
    y = s_state.mouse_previous.y;
}


