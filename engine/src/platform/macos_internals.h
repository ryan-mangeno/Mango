#pragma once

#include "defines.h"

#if MGO_PLATFORM_APPLE

#include "defines.h"
#import <Cocoa/Cocoa.h>

@interface AppleInputView : NSView
@end

@interface AppleWindowDelegate : NSObject <NSWindowDelegate>
@property(assign) b8 *quit_flag;
@end

struct InternalState {
  NSWindow *window;
  AppleWindowDelegate *delegate;
  AppleInputView *input_view;
  b8 quit_requested;
};

AppleInputView *create_apple_input();

#endif
