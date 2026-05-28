#include "event.h"
#include "mgmemory.h"

#include "containers/darray.h"

// TODO: 1. this can be multi threaded 
//       2. maybe add priority 
//       3. we can set a limit on how many events fired per frame ( event queue ) 
//          ... jobified


/* Layout Example

EVENT SYSTEM LOOKUP TABLE 
───────────────────────────────────────────────────────────────────────────
Slot [0]: (EMPTY)
───────────────────────────────────────────────────────────────────────────
Slot [1] (EVENT_CODE_APPLICATION_QUIT):
   └── darray: [ (Listener: Application_Instance_A, Callback: on_event) ]
───────────────────────────────────────────────────────────────────────────
Slot [2] (EVENT_CODE_KEY_PRESSED):
   └── darray: [ 
                 (Listener: Application_Instance_A, Callback: on_key),
                 (Listener: Player_1_Instance,      Callback: jump),
                 (Listener: UI_Menu_Instance,       Callback: play_sound)
               ]
───────────────────────────────────────────────────────────────────────────
Slot [3] to [127]: (EMPTY)

*/

struct registered_event {
    void* listener;
    EventCallback callback;

    registered_event(void* lis, EventCallback cb) : listener(lis), callback(cb) {}
};

struct event_code_entry {
    darray<registered_event> events_;
};

#define MAX_MESSAGE_CODES 128

struct event_system_state {
    // lookup table for event codes
    event_code_entry registered_[MAX_MESSAGE_CODES];
};

static b8 s_is_initialized = FALSE;
static event_system_state* s_state = nullptr;

b8 event_initialize() {
    if (s_is_initialized == TRUE) {
        return FALSE;
    }

    s_state = mg_new<event_system_state>(MEMORY_TAG_APPLICATION);

    s_is_initialized = TRUE;
    return TRUE;
}

void event_shutdown() {
    if (s_is_initialized == FALSE || s_state == nullptr) {
        return;
    }

    mg_delete(s_state, MEMORY_TAG_APPLICATION);
    s_state = nullptr;

    s_is_initialized = FALSE;
}


b8 event_register(u16 code, void* listener, EventCallback callback) {
    if (s_is_initialized == FALSE) {
        return FALSE;
    }   

    auto& events = s_state->registered_[code].events_;
    u64 count = events.size();
    for (u64 i=0 ; i<count ; ++i) {
        if (events[i].listener == listener) {
            MGO_WARN("Tried to register duplicate event...");
            return FALSE;
        }
    }

    // no duplicate was found, proceed to callback
    events.emplace_back(listener, callback);

    return TRUE;
}

b8 event_unregister(u16 code, void* listener, EventCallback callback) {
    if (s_is_initialized == FALSE) {
        return FALSE;
    }   

    auto& events = s_state->registered_[code].events_;
    u64 count = events.size();

    if (count == 0) {
        MGO_WARN("No events to unregister.");
        return FALSE;
    }

    for (u64 i = 0; i<events.size(); ) {
        const registered_event& e = events[i];
        if (e.listener == listener && e.callback == callback) {
            events.erase(i);
            return TRUE;
        }        
    }

    // Not found
    return FALSE;
}

b8 event_fire(u16 code, void* sender, EventContext ctx) {
    if (s_is_initialized == FALSE) {
        return FALSE;
    }

    auto& events = s_state->registered_[code].events_;
    u64 count = events.size();

    if (count == 0) {
        // TODO: warn
        return FALSE;
    }

    for (u64 i=0 ; i<count ; ++i) {
        const registered_event& e = events[i];
        // allow callbacks to stop propogation to other listeners
        if (e.callback(code, e.listener, sender, ctx)) {
            return TRUE;
        }
    }

    return FALSE;
}
