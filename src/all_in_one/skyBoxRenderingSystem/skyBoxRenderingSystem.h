#pragma once
#include "ecs/system.h"
#include "ecs/world.h"
#include "renderers/skyBoxRenderer.h"
struct SkyBoxTag{};
class SkyBoxRenderingSystem : public System {
 public:
  SkyBoxRenderingSystem() {}
  virtual ~SkyBoxRenderingSystem();
  void start(World& world) override;
  void update(float dt, World& world) override;
  void render(World& world) override;

 private:
  std::unique_ptr<SkyBoxRenderer> renderer_{nullptr};
};