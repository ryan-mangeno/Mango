#pragma once

#include "defines.h"

class Clock {
public:
  Clock() = default;

  // also resets elapsed time
  inline void start() {
    m_start_time = Platform::get_absolute_time();
    m_elapsed = 0.0;
  }

  // should be called before checking elapsed time
  inline void update() {
    if (m_start_time != 0.0) {
      f64 now_time = Platform::get_absolute_time();
      m_elapsed = now_time - m_start_time;
    }
  }

  // does not reset elapsed time
  inline void stop() noexcept { m_start_time = 0.0; }

  inline void set_last_time(f64 last_time) noexcept { m_last_time = last_time; }
  inline f64 get_last_time() const noexcept { return m_last_time; }
  inline f64 get_elapsed_time() const noexcept { return m_elapsed; }

private:
  f64 m_start_time;
  f64 m_elapsed; // NOTE: in seconds
  f64 m_last_time;
};
