#include "platform.h"
#include <core/logger.h>

#if MGO_PLATFORM_APPLE

#import <Cocoa/Cocoa.h> 

#include <stdlib.h>
#include <time.h>


@interface AppleWindowDelegate : NSObject <NSWindowDelegate>
@property (assign) Mango::b8* quit_flag;
@end

@implementation AppleWindowDelegate
- (void)windowWillClose:(NSNotification *)notification {
    if (self.quit_flag) {
        *(self.quit_flag) = true; 
    }
}
@end

namespace Mango {

    struct InternalState {
        NSWindow* window;
        NSAutoreleasePool* pool;
        AppleWindowDelegate* delegate;
        b8 quit_requested;
    };

    static f64 start_time = 0;

    PlatformState::PlatformState() {
        
    }

    PlatformState::~PlatformState() {

    }

    b8 PlatformState::startup(const AppAttribs& attribs) {

        state_ = static_cast<InternalState*>(malloc(sizeof(InternalState)));
        state_->quit_requested = false;
        state_->pool = [[NSAutoreleasePool alloc] init];
        state_->delegate = [[AppleWindowDelegate alloc] init]; // delegate for event callback
        state_->delegate.quit_flag = &state_->quit_requested;

        // initializes app singleton
        [NSApplication sharedApplication];

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

        [state_->window setTitle:[NSString stringWithUTF8String:attribs.title]];

        // tells macos to render window and bring to front
        [state_->window makeKeyAndOrderFront:nil];

        // register delegate with window
        [state_->window setDelegate:state_->delegate];

        // signals done with setup
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];

        start_time = get_absolute_time();

        return true;
    }

    void PlatformState::shutdown() {

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
        ts.tv_nsec = (ms % static_cast<u64>(1e3)) * static_cast<f64>(1e9);

        nanosleep(&ts, nullptr);
    }

}

#endif