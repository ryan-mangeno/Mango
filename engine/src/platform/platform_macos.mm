#include "platform.h"

#include <core/logger.h>

#if MGO_PLATFORM_APPLE

#import <Cocoa/Cocoa.h> 


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

                MGO_INFO("-Processing Event-");
            }
        }

        return state_->quit_requested;
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