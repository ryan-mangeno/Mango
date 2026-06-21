#pragma once

#include "defines.h"

enum class GraphicsAPI {
  None,
  Vulkan,
  OpenGL,
  DirectX,
};

struct RenderPacket {
  f32 delta_time;
};
