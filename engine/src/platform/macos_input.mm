#include <defines.h>

#if MGO_PLATFORM_APPLE

#include "macos_internals.h"
#include <core/input.h>


AppleInputView* create_apple_input() {
    return [[AppleInputView alloc] init];
}

static keys translate_keycode(u16 ns_keycode) {
    switch (ns_keycode) {
        case 0x00: return KEY_A;
        case 0x0B: return KEY_B;
        case 0x08: return KEY_C;
        case 0x02: return KEY_D;
        case 0x0E: return KEY_E;
        case 0x03: return KEY_F;
        case 0x05: return KEY_G;
        case 0x04: return KEY_H;
        case 0x22: return KEY_I;
        case 0x26: return KEY_J;
        case 0x28: return KEY_K;
        case 0x25: return KEY_L;
        case 0x2E: return KEY_M;
        case 0x2D: return KEY_N;
        case 0x1F: return KEY_O;
        case 0x23: return KEY_P;
        case 0x0C: return KEY_Q;
        case 0x0F: return KEY_R;
        case 0x01: return KEY_S;
        case 0x11: return KEY_T;
        case 0x20: return KEY_U;
        case 0x09: return KEY_V;
        case 0x0D: return KEY_W;
        case 0x07: return KEY_X;
        case 0x10: return KEY_Y;
        case 0x06: return KEY_Z;

        // Numbers
        case 0x1D: return KEY_NUMPAD0;
        case 0x12: return KEY_NUMPAD1;
        case 0x13: return KEY_NUMPAD2;
        case 0x14: return KEY_NUMPAD3;
        case 0x15: return KEY_NUMPAD4;
        case 0x17: return KEY_NUMPAD5;
        case 0x16: return KEY_NUMPAD6;
        case 0x1A: return KEY_NUMPAD7;
        case 0x1C: return KEY_NUMPAD8;
        case 0x19: return KEY_NUMPAD9;

        // Special keys
        case 0x31: return KEY_SPACE;
        case 0x35: return KEY_ESCAPE;
        case 0x24: return KEY_ENTER;
        case 0x30: return KEY_TAB;
        case 0x33: return KEY_BACKSPACE;
        case 0x75: return KEY_DELETE;

        // Arrows
        case 0x7E: return KEY_UP;
        case 0x7D: return KEY_DOWN;
        case 0x7B: return KEY_LEFT;
        case 0x7C: return KEY_RIGHT;

        // Function keys
        case 0x7A: return KEY_F1;
        case 0x78: return KEY_F2;
        case 0x63: return KEY_F3;
        case 0x76: return KEY_F4;
        case 0x60: return KEY_F5;
        case 0x61: return KEY_F6;
        case 0x62: return KEY_F7;
        case 0x64: return KEY_F8;
        case 0x65: return KEY_F9;
        case 0x6D: return KEY_F10;
        case 0x67: return KEY_F11;
        case 0x6F: return KEY_F12;

        // Modifiers (Note: macOS handles these differently in modifierFlags,
        // but these are the physical key down codes)
        case 0x38: return KEY_LSHIFT;
        case 0x3C: return KEY_RSHIFT;
        case 0x3B: return KEY_LCONTROL;
        case 0x3E: return KEY_RCONTROL;
        case 0x3A: return KEY_LMENU; // Option/Alt
        case 0x3D: return KEY_RMENU; // Right Option

        default: return KEYS_MAX_KEYS;
    }
}


@implementation AppleInputView
// required for keyboard input to work
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)canBecomeKeyView { return YES; }

- (void)keyDown:(NSEvent *)event {
    // TODO: Input system fire key_pressed
}

- (void)keyUp:(NSEvent *)event {
    keys key_code = translate_keycode([event keyCode]);
    // TODO: Input system fire key_released, same for rest 
}

- (void)mouseDown:(NSEvent *)event { /* Left */ 
}

- (void)rightMouseDown:(NSEvent *)event { /* Right */ 
}

- (void)mouseUp:(NSEvent *)event { 
}

- (void)mouseMoved:(NSEvent *)event {
    // NSPoint location = [event locationInWindow];
}

- (void)scrollWheel:(NSEvent *)event {
    // f32 delta_y = [event scrollingDeltaY];
}
@end


#endif