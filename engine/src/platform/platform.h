#pragma once

#include <defines.h>
#include <core/logger.h>

// #include <stdx/functional.h>

struct InternalState; // defined per platform

struct AppConfig {
	const char* title;

	u32 x;
	u32 y;
	u32 width;
	u32 height;

	AppConfig(const char* app_name = "Mango Engine",
		u32 x = 0,
		u32 y = 0,
		u32 w = 1280,
		u32 h = 720)
		: title(app_name), width(w), height(h), x(x), y(y)
	{}
};

class Platform {
	public:
		~Platform() {};
		Platform() {};

		b8 is_running();
		b8 startup(const AppConfig& attribs = AppConfig());
		void shutdown();

		// returns a quit request boolean
		b8 pump_message();

		// platform specific memory management and utilities

		static void* allocate(u64 size, b8 aligned) noexcept;
		static void  free(void* block, b8 aligned) noexcept;
		static void* zero_memory(void* block, u64 size) noexcept;
		static void* copy_memory(void* dest, const void* source, u64 size) noexcept;
		static void* set_memory(void* dest, i32 value, u64 size) noexcept;

		static void console_write(const char* message, log_level color);
		static void console_write_error(const char* message, log_level color);

		static f64 get_absolute_time() noexcept;

		static void sleep(u64 ms) noexcept;
	
	private:
		InternalState* m_internal_state = nullptr;
};
