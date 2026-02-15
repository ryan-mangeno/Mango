#include <defines.h>

#if MGO_PLATFORM_APPLE

#include "platform.h"
#include "macos_internals.h"

#include <core/assert.h>
#include <core/logger.h>

#include <stdlib.h>
#include <time.h>


static f64 start_time = 0;

b8 Platform::is_running() { return !internal_state_->quit_requested; }

b8 Platform::startup(const AppConfig& config) {

    // TODO: replace with own allocator
    internal_state_ = static_cast<InternalState*>(malloc(sizeof(InternalState)));

    InternalState* state = internal_state_;

    if (!state) {
        return FALSE;
    }

    zero_memory(state, sizeof(InternalState));
    state->quit_requested = FALSE;

    state->delegate = [[AppleWindowDelegate alloc] init]; 
    if (!state->delegate) {
        return FALSE;
    }
    state->delegate.quit_flag = &state->quit_requested;

    state->input_view = create_apple_input();
    if (!state->input_view) {
        return FALSE;
    }

    // initializes app singleton
    if (![NSApplication sharedApplication]) {
        return FALSE;
    }
    
    MGO_ASSERT_MSG([NSThread isMainThread], "app must exist on the main thread!");

    // shows in dock, has a menu, focus
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    u32 style_mask = NSWindowStyleMaskTitled | 
                        NSWindowStyleMaskClosable | 
                        NSWindowStyleMaskMiniaturizable | 
                        NSWindowStyleMaskResizable;
    NSRect frame = NSMakeRect(config.x, config.y, config.width, config.height);
    
    state->window = [[NSWindow alloc] initWithContentRect:frame
                                        styleMask:style_mask
                                        backing:NSBackingStoreBuffered
                                        defer:NO];
    [state->window setReleasedWhenClosed:NO];
    
    if (!state->window) {
        return FALSE;
    }

    [state->window setTitle:[NSString stringWithUTF8String:config.title]];

    // tells macos to render window and bring to front
    [state->window makeKeyAndOrderFront:nil];

    // register delegate with window
    [state->window setDelegate:state->delegate];

    // register input view
    [state->window setContentView:state->input_view];
    [state->window makeFirstResponder:state->input_view];
    // enable mouse moves which is disabled by default
    [state->window setAcceptsMouseMovedEvents:YES];


    // signals done with setup
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp finishLaunching];

    start_time = get_absolute_time();

    return TRUE;
}

void Platform::shutdown() {
    InternalState* state = static_cast<InternalState*>(internal_state_);

    if (state->window) {
        [state->window setDelegate:nil]; // Prevent delegate callbacks during teardown
        [state->window close];           // This hides and potentially releases
        [state->window release];
        state->window = nullptr;       
    }

    [state->delegate release]; // safe if nullptr
    [state->input_view release]; // same here

    state->delegate = nullptr;
    state->input_view = nullptr;

    // NOTE: c free not Platform::free 
    // TODO: replace with own allocator
    ::free(internal_state_);
}

b8 Platform::pump_message() {

    // auto release pool needs to be made since events creating
    // during pump can leak memory

    // TODO: propogate events below in callbacks 
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

    return internal_state_->quit_requested;
}

void* Platform::allocate(u64 size, b8 aligned) {
    return malloc(size); // TODO: add aligned logic
}   

void Platform::free(void* block, b8 aligned) {
    ::free(block); // TODO: add aligned logic
}

void* Platform::zero_memory(void* block, u64 size) {
    return memset(block, 0, size);
}

void* Platform::copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

void* Platform::set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}   

void Platform::console_write(const char* message, log_level color) {
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
    fprintf(stdout, "%s%s%s\n", levels[static_cast<int>(color)], message, reset);
    fflush(stdout);
}

void Platform::console_write_error(const char* message, log_level color) {
    static const char* levels[6] = {
        "\033[41m", // FATAL 
        "\033[31m", // ERROR 
        "\033[33m", // WARN  
        "\033[32m", // INFO  
        "\033[34m", // DEBUG 
        "\033[37m"  // TRACE 
    };
    
    const char* reset = "\033[0m";

    fprintf(stderr, "%s%s%s\n", levels[static_cast<int>(color)], message, reset);
    fflush(stderr);
}

f64 Platform::get_absolute_time() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    f64 now_time = static_cast<f64>(now.tv_sec) + static_cast<f64>(now.tv_nsec) * static_cast<f64>(1e-9);

    return now_time - start_time;
}

void Platform::sleep(u64 ms) {
    struct timespec ts;
    ts.tv_sec  = ms / static_cast<u64>(1e3);
    ts.tv_nsec = (ms % static_cast<u64>(1e3)) * static_cast<f64>(1e6);

    nanosleep(&ts, nullptr);
}



// TODO: ensure delegates callback impls are destroyed before platform is
@implementation AppleWindowDelegate
- (void)windowWillClose:(NSNotification *)notification {
    if (self.quit_flag) {
        *(self.quit_flag) = TRUE; 
    }
}

- (void)windowDidResize:(NSNotification *)notification {
    // TODO: handle new dims     
}

- (void)dealloc {
    MGO_WARN("AppleWindowDelegate is being DEALLOCATED (freed from memory)!");
    [super dealloc];
}
@end


#endif