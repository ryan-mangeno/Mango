#include "event.h"
#include "mgmemory.h"

#include <containers/darray.h>

struct registered_event {
    void* listener_;
    EventCallback callback_;

    registered_event(void* lis, EventCallback cb) : listener_(lis), callback_(cb) {}
};

struct event_code_entry {
    darray<registered_event> events_;
};

#define MAX_MESSAGE_CODES 128

struct event_system_state {
    // lookup table for event codes
    event_code_entry registered_[MAX_MESSAGE_CODES];
};

static b8 is_inialized_ = FALSE;
static event_system_state state_;

b8 event_initialize() {
    if (is_inialized_ == TRUE) {
        return FALSE;
    }
    is_inialized_ = TRUE;
    return TRUE;
}

void event_shutdown() {
    // free events arr, and objects pointer to should be destroyed on their own
    state_ = {};
    is_inialized_ = FALSE;
}


b8 event_register(u16 code, void* listener, EventCallback callback) {
    if (is_inialized_ == FALSE) {
        return FALSE;
    }   

    u64 count = state_.registered_[code].events_.size();
    for (u64 i=0 ; i<count ; ++i) {
        if (state_.registered_[code].events_[i].listener_ == listener) {
            // TODO: warn
            return FALSE;
        }
    }

    // no duplicate was found, proceed to callback
    state_.registered_[code].events_.emplace_back(listener, callback);

    return TRUE;
}

b8 event_unregister(u16 code, void* listener, EventCallback callback) {
    if (is_inialized_ == FALSE) {
        return FALSE;
    }   

    if (state_.registered_[code].events_.size() == 0) {
        // TODO: warn
        return FALSE;
    }

    u64 count = state_.registered_[code].events_.size();
    for (u64 i=0 ; i<count ; ++i) {
        const registered_event& e = state_.registered_[code].events_[i];
        if (e.listener_ == listener && e.callback_ == callback) {
            state_.registered_[code].events_.pop_back();
            return TRUE;
        }        
    }

    // Not found
    return FALSE;
}
