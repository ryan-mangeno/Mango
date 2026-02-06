#pragma once

#include <defines.h>

namespace Mango {

	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication     = BIT(0),
		EventCategoryInput           = BIT(1),
		EventCategoryKeyboard        = BIT(2),
		EventCategoryMouse           = BIT(3),
		EventCategoryMouseButton     = BIT(4),
	};


    #define EVENT_CLASS_TYPE(type) static EventType static_type() { return EventType::type; }\
							    virtual EventType event_type() const override { return static_type(); }\
								virtual const char* name() const override { return #type; }


    #define EVENT_CLASS_CATEGORY(category) virtual int category_flags() const override { return category; }


    class Event {
        public:
            virtual EventType event_type() const = 0;
            virtual const char* name() const = 0;
            virtual int category_flags() const = 0;
            
            inline bool handled_state() const { return handled_; }
            inline bool check_in_category(EventCategory category) const {
                return category_flags() & category; // NOTE: bitfield used above
            }

        protected:
            bool handled_ = false;
	};


    // NOTE: TEST EVENT
    class MousePressEvent : public Event {
        public:
            EVENT_CLASS_TYPE(MouseButtonPressed)
            EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
        private:
    };

} // namespace Mango