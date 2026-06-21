#pragma once

#include "renderer/renderer_api.h"

#include "containers/darray.h"

// TODO: fill out
class VulkanAPI : public RendererAPI {
public:
  VulkanAPI() = default;
  ~VulkanAPI() = default;

  virtual b8 init(const char *app_name) override;
  virtual void shutdown() override;
  virtual void resized(u16 width, u16 height) override;
  virtual b8 begin_frame(f32 delta_time) override;
  virtual b8 end_frame(f32 delta_time) override;

private:
  void _get_required_extensions(darray<const char *> &extensions);
};
