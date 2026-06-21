#pragma once

#include "renderer_api.h"
#include "renderer_types.h"

class RendererBackend {
public:
  static RendererAPI *create();
};
