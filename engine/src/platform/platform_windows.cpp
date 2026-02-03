#include "platform.h"

#if MGO_PLATFORM_WINDOWS

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

namespace Mango {

    struct InternalState {
        HINSTANCE h_instance;
        HWND hwnd;
    };

    PlatformState::PlatformState() {
        
    }

    PlatformState::~PlatformState() {

    }

    b8 PlatformState::startup(const AppAttribs& attribs) {
        state_ = static_cast<InternalState*>malloc(sizeof(InternalState));     
        
        state_->h_instance = GetModuleHandle(0);

        // setup and register window class
        HICON icon = LoadIcon(state_->h_instance, IDI_APPLICATION);
        WNDCLASSA wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_DBCLKS; // get double clicks
        wc.lpfnWndProc = win32_process_message; // for handling events
        wc.cbClsExtra = 0;
        wc.cvWndExtra = 0;
        wc.hInstance = state_->h_instance;
        wc.hIcon = icon;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW); // manage the cursor manually
        wc.hbrBackround = NULL; // transparent
        wc.lpszClassName = "mango_window_class";

        if (!RegisterClass(&wc)) {
            MessageBoxA(0, "window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
            return false;
        }

        const auto [name, x, y, width, height] = attribs;

        // the window includes the frame and active window area
        // the outer frame is like the x button, meta data, etc
        // we must handle both seperatly

        u32 client_x = x;
        u32 client_y = y;
        u32 client_width = width;
        u32 client_height = height;

        u32 window_x = x;
        u32 window_y = y;
        u32 window_width = width;
        u32 window_height = height;

        u32 window_style    = WS_OVERLAPPED 
                            | WS_SYSMENU 
                            | WS_CAPTION
                            | WS_MAXIMIZEBOX
                            | WS_MINIMIZEBOX
                            | WS_THICKFRAME;

        u32 window_ex_style = WS_EX_APPWINDOW;

        // get the offset of the window frame and client area
        RECT border_rect = {0,0,0,0};
        AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

        // push out window x and y by the border offset
        window_x += border_rect.left;
        window_y += border_rect.top;

        // grow by size of os border
        window_width  += border_rect.right  - border_rect.left;
        window_height += border_rect.bottom - border_rect.top;

        HWND handle = CreateWindowExA(
            window_ex_style, "mango_window_class", name,
            window_style, window_x, window_y, window_width, window_height,
            0, 0, state->h_instance, 0);

        if (handle == 0) {
            MessageBoxA(0, "window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
            MGO_FATAL("window creation failed!");
            return false;
        }
        else {
            state_->hwnd = handle;
        }

        // TODO: show window

        return true;
    }

    void PlatformState::shutdown() {

    }

    b8 PlatformState::pump_message() {

    }

    void* PlatformState::allocate(u64 size, b8 aligned) {
        return nullptr;
    }

    void PlatformState::free(void* block, b8 aligned) {

    }

    void* PlatformState::zero_memory(void* block, u64 size) {
        return nullptr;
    }

    void* PlatformState::copy_memory(void* dest, void* source, u64 size) {
        return nullptr;
    }

    void* PlatformState::set_memory(void* dest, i32 value, u64 size) {
        return nullptr;
    }   

    void PlatformState::console_write(const char* message, u8 color) {

    }

    void PlatformState::console_write_error(const char* message, u8 color) {

    }

    f64 PlatformState::get_absolute_time() {
        return 0.0f;
    }

    void PlatformState::sleep(u64 ms) {

    }

}

#endif