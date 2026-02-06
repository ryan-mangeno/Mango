#include "platform.h"

#if MGO_PLATFORM_WINDOWS

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

// defined at bottom
LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);
namespace Mango {

    struct InternalState {
        HINSTANCE h_instance;
        HWND hwnd;
    };

    static f64 clock_frequency; // multiplier to take clock cycles and multiply by this to get start time
    static LARGE_INTEGER start_time;

    PlatformState::PlatformState() {
        
    }

    PlatformState::~PlatformState() {

    }

    // TODO: should add running bool to state
    b8 PlatformState::is_running() { return true; }

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

        b8 should_activate = true; // TODO: if the window should not accept input, this should be false
        i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
        // if initially minimized use SW_MINIMIZE : SW_SHOWMINNOACTIVE
        // if initially maximized use SW_SHOWMAXIMIZED : SW_MAXIMIZE
        ShowWindow(state_->hwnd, show_window_command_flags);

        // clock setup
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        clock_frequency = 1.0 / static_cast<f64>(frequency.QuadPart);
        QueryPerformanceCounter(&start_time);

        return true;
    }

    void PlatformState::shutdown() {

        if (state_->hwnd) {
            DestroyWindow(state_hwnd);
            state_->hwnd = nullptr;
        }

    }
    
    b8 PlatformState::pump_message() {
        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        return true;
    }

    void* PlatformState::allocate(u64 size, b8 aligned) {
        return malloc(size);
    }   

    void PlatformState::free(void* block, b8 aligned) {
        free(block);
    }

    void* PlatformState::zero_memory(void* block, u64 size) {
        return memset(block, 0, size);
    }

    void* PlatformState::copy_memory(void* dest, void* source, u64 size) {
        return memcpy(dest, source, size);
    }

    void* PlatformState::set_memory(void* dest, i32 value, u64 size) {
        return memset(dest, value, size);
    }   

    void PlatformState::console_write(const char* message, log_level color) {
        HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
        static u8 levels[6] = {64, 4, 6, 2, 1, 8};
        SetConsoleTextAttribute(console_handle, levels[color]);

        OutputDebugStringA(message);
        u64 len = strlen(message);
        LPDWORD number_written = 0;
        WriteConsoleA(console_handle, message, (DWORD)len, number_written, 0);
    }

    void PlatformState::console_write_error(const char* message, log_level color) {
        HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
        static u8 levels[6] = {64, 4, 6, 2, 1, 8};
        SetConsoleTextAttribute(console_handle, levels[color]);

        OutputDebugStringA(message);
        u64 len = strlen(message);
        LPDWORD number_written = 0;
        WriteConsoleA(console_handle, message, (DWORD)len, number_written, 0);
    }

    f64 PlatformState::get_absolute_time() {
        LARGE_INTEGER now_time;
        QueryPerformanceCounter(&now_time);
        return static_cast<f64>(now_time.QuadPart * clock_frequency);
    }

    void PlatformState::sleep(u64 ms) {
        Sleep(ms);
    }

}


LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_ERASEBKGND:
            // notify the os erasing will be handled by app to prevent flicker
            return 1;
        case WM_CLOSE:
            // TODO: fire an event for app to quit
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: fire window resize event
        }   break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: input processing
        }   break;
        case WM_MOUSEMOVE: {
            // i32 x = GET_X_LPARAM(l_param);
            // i32 y = GET_Y_LPARAM(l_param);
            // TODO: input processing;
        }   break;
        case WM_MOUSEHWEEL: {
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if (z_delta != 0) {
            //    // flaten input to os-indipendent (-1, 1)
            //     z_delta = (z_delta < 0) ? -1 : 1; // just want to know if its up or down;
            // }
            // TODO: input processing;
        }   break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            // b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN;
            // TODO: input processing;
        }   break;

        return DefWindowProcA(hwnd, msg, w_param, l_param);
    }
}

#endif