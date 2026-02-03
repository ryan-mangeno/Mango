#pragma once

#include <defines.h>

// #include <stdx/functional.h>

namespace Mango {

	struct InternalState; // defined per platform

	struct AppAttribs {
		const char* title;

		u32 x;
		u32 y;
		u32 width;
		u32 height;

		AppAttribs(const char* app_name = "Mango Engine",
			u32 x = 0,
			u32 y = 0,
			u32 w = 1280,
			u32 h = 720)
			: title(app_name), width(w), height(h), x(x), y(y)
		{}
	};

	class PlatformState {
		public:
			PlatformState();
			~PlatformState();

			b8	 startup(const AppAttribs& attribs = AppAttribs());
			void shutdown();

			b8 pump_message();

			void* allocate(u64 size, b8 aligned);
			void  free(void* block, b8 aligned);
			void* zero_memory(void* block, u64 size);
			void* copy_memory(void* dest, void* source, u64 size);
			void* set_memory(void* dest, i32 value, u64 size);

			void console_write(const char* message, u8 color);
			void console_write_error(const char* message, u8 color);

			f64 get_absolute_time();

			void sleep(u64 ms);
		
		protected:
			InternalState* state_ = nullptr;
	};


}