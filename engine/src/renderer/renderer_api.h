#pragma once

#include "defines.h"

#include "renderer_types.h"
class RendererAPI {
public:
  virtual ~RendererAPI() = default;

  virtual b8 init(const char *app_name) = 0;
  virtual void shutdown() = 0;
  virtual void resized(u16 width, u16 height) = 0;
  virtual b8 begin_frame(f32 delta_time) = 0;
  virtual b8 end_frame(f32 delta_time) = 0;

  inline static GraphicsAPI get_api() { return s_api; }

private:
  static GraphicsAPI s_api;
};
