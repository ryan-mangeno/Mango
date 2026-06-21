#pragma once
#include "core/application.h"

struct game {
  // Game-specific state, managed by the client
  void *state_ = nullptr;
  AppConfig app_config{};

  // func pointers to game callbacks
  b8 (*initialize)(game *game_inst) = nullptr;
  b8 (*update)(game *game_inst, f64 delta_time) = nullptr;
  b8 (*render)(game *game_inst, f64 delta_time) = nullptr;
  b8 (*on_resize)(game *game_inst, f64 delta_time) = nullptr;
};
