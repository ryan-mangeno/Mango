#pragma once

#include <defines.hpp>

namespace Mango {

	struct WindowAttribs {
		const char* title;
		u32 width;
		u32 height;

		WindowAttribs(const char* app_name = "Mango Engine",
			u32 w = 1280,
			u32 h = 720)
			: title(app_name), width(w), height(h)
		{}
	};


	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void on_update() = 0;

		virtual u32 width() const = 0;
		virtual u32 height() const = 0;

		virtual void set_event_callback(const EventCallbackFn& callback) = 0;
		virtual void set_vsync(bool enabled) = 0;
		virtual bool is_vsync() const = 0;

		// void* allows us to cast to any other type of window depending on platform
		virtual void* get_native_window() const = 0;

		static Window* create(const WindowAttribs& attribs = WindowAttribs());
	};
}