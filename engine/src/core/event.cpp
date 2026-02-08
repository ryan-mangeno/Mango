#include "event.h"
#include "mgmemory.h"

struct registered_event {
    void* listener_;
    EventCallback callback_;
};

struct event_code_entry {
    registered_event* registered_events_;
};

#define MAX_MESSAGE_CODES 16384

struct event_system_state {
    // lookup table for event codes
    event_code_entry registered_[MAX_MESSAGE_CODES];
};

static b8 is_inialized_ = FALSE;
static event_system_state state_;

