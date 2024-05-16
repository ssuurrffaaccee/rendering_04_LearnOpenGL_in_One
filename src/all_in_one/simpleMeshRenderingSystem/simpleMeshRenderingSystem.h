#pragma once
#include "ecs/system.h"
#include "ecs/world.h"
#include "renderers/simpleMeshRenderer.h"
class SimpleMeshRenderingSystem : public System {
 public:
  SimpleMeshRenderingSystem() {}
  virtual ~SimpleMeshRenderingSystem();
  void start(World& world) override;
  void update(float dt, World& world) override;
  void render(World& world) override;

 private:
  std::unique_ptr<SimpleMeshRenderer> renderer_{nullptr};
};