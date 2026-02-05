#include "platform.h"

#include <core/assert.h>
#include <core/logger.h>

#if MGO_PLATFORM_APPLE

#import <Cocoa/Cocoa.h> 

#include <stdlib.h>
#include <time.h>


// input handling and delegates at the bottom
@interface AppleWindowDelegate : NSObject <NSWindowDelegate>
@property (assign) Mango::b8* quit_flag;
@end

@interface AppleInputView : NSView
// TODO: add properties potentially
@end

namespace Mango {

    struct InternalState {
        NSWindow* window;
        AppleWindowDelegate* delegate;
        AppleInputView* input_view;
        b8 quit_requested;
    };

    static f64 start_time = 0;

    PlatformState::PlatformState() {
        
    }

    PlatformState::~PlatformState() {

    }

    b8 PlatformState::is_running() { return !state_->quit_requested; }

    b8 PlatformState::startup(const AppAttribs& attribs) {

        state_ = static_cast<InternalState*>(malloc(sizeof(InternalState)));

        if (!state_) {
            return false;
        }

        zero_memory(state_, sizeof(InternalState));
        state_->quit_requested = false;

        state_->delegate = [[AppleWindowDelegate alloc] init]; 
        if (!state_->delegate) {
            return false;
        }
        state_->delegate.quit_flag = &state_->quit_requested;

        state_->input_view = [[AppleInputView alloc] init];
        if (!state_->input_view) {
            return false;
        }

        // initializes app singleton
        if (![NSApplication sharedApplication]) {
            return false;
        }
        
        MGO_ASSERT_MSG([NSThread isMainThread], "app must exist on the main thread!");

        // shows in dock, has a menu, focus
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        u32 style_mask = NSWindowStyleMaskTitled | 
                         NSWindowStyleMaskClosable | 
                         NSWindowStyleMaskMiniaturizable | 
                         NSWindowStyleMaskResizable;
        NSRect frame = NSMakeRect(attribs.x, attribs.y, attribs.width, attribs.height);
        
        state_->window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:style_mask
                                          backing:NSBackingStoreBuffered
                                          defer:NO];
        
        if (!state_->window) {
            return false;
        }

        [state_->window setTitle:[NSString stringWithUTF8String:attribs.title]];

        // tells macos to render window and bring to front
        [state_->window makeKeyAndOrderFront:nil];

        // register delegate with window
        [state_->window setDelegate:state_->delegate];

        // register input view
        [state_->window setContentView:state_->input_view];
        [state_->window makeFirstResponder:state_->input_view];
        // enable mouse moves which is disabled by default
        [state_->window setAcceptsMouseMovedEvents:YES];


        // signals done with setup
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];

        start_time = get_absolute_time();

        return true;
    }

    void PlatformState::shutdown() {

        if (state_->window) {
            [state_->window setDelegate:nil]; // Prevent delegate callbacks during teardown
            [state_->window close];           // This hides and potentially releases
            state_->window = nullptr;       
        }

        [state_->delegate release]; // safe if nullptr
        [state_->input_view release]; // same here

        state_->delegate = nullptr;
        state_->input_view = nullptr;

        // NOTE: c free not platformstate::free 
        ::free(state_);
    }

    b8 PlatformState::pump_message() {

        // auto release pool needs to be made since events creating
        // during pump can leak memory

        // TODO: look into how events can be propogated 
        @autoreleasepool {
            NSEvent* event;

            // pull all pending events
            while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                   untilDate:[NSDate distantPast] // dont block
                                   inMode:NSDefaultRunLoopMode
                                   dequeue:YES])) {
                [NSApp sendEvent:event]; // sends event to window
                [NSApp updateWindows];   // notifies os we are still processing
            }
        }

        return state_->quit_requested;
    }

    void* PlatformState::allocate(u64 size, b8 aligned) {
        return malloc(size); // TODO: add aligned logic
    }   

    void PlatformState::free(void* block, b8 aligned) {
        ::free(block); // TODO: add aligned logic
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
        static const char* levels[6] = {
        "\033[41m", // FATAL (White on Red background)
        "\033[31m", // ERROR (Red text)
        "\033[33m", // WARN  (Yellow text)
        "\033[32m", // INFO  (Green text)
        "\033[34m", // DEBUG (Blue text)
        "\033[37m"  // TRACE (Grey text)
        };
    
        const char* reset = "\033[0m";

        // TODO: should look into templating for converting log_level to i8 statically
        // but since its just logging primarily for debug
        fprintf(stdout, "%s%s%s", levels[static_cast<int>(color)], message, reset);
        fflush(stdout);
    }

    void PlatformState::console_write_error(const char* message, log_level color) {
        static const char* levels[6] = {
            "\033[41m", // FATAL 
            "\033[31m", // ERROR 
            "\033[33m", // WARN  
            "\033[32m", // INFO  
            "\033[34m", // DEBUG 
            "\033[37m"  // TRACE 
        };
        
        const char* reset = "\033[0m";

        fprintf(stderr, "%s%s%s", levels[static_cast<int>(color)], message, reset);
        fflush(stderr);
    }

    f64 PlatformState::get_absolute_time() {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        f64 now_time = static_cast<f64>(now.tv_sec) + static_cast<f64>(now.tv_nsec) * static_cast<f64>(1e-9);

        return now_time - start_time;
    }

    void PlatformState::sleep(u64 ms) {
        struct timespec ts;
        ts.tv_sec  = ms / static_cast<u64>(1e3);
        ts.tv_nsec = (ms % static_cast<u64>(1e3)) * static_cast<f64>(1e6);

        nanosleep(&ts, nullptr);
    }

}


// TODO: ensure delegates callback impls are destroyed before platform is
@implementation AppleWindowDelegate
- (void)windowWillClose:(NSNotification *)notification {
    if (self.quit_flag) {
        *(self.quit_flag) = true; 
    }
}

- (void)windowDidResize:(NSNotification *)notification {
    // TODO: handle new dims     
}
@end


@implementation AppleInputView
// required for keyboard input to work
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)canBecomeKeyView { return YES; }

- (void)keyDown:(NSEvent *)event {
    // u16 key_code = [event keyCode];
    // TODO: Input system fire key_pressed
}

- (void)keyUp:(NSEvent *)event {
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