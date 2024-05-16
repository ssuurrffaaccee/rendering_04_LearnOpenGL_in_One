#pragma once
#include "ecs/system.h"
#include "ecs/world.h"
#include "renderers/deferredShadingRenderer.h"
#include "renderers/shadowMapDebugRenderer.h"
#include "renderers/spriteRenderer.h"
#include "window/windowSize.h"
class DeferredShadingSystem : public System {
 public:
  DeferredShadingSystem() {}
  virtual ~DeferredShadingSystem() {}
  void start(World& world);
  void update(float dt, World& world);
  void render(World& world);

 private:
  void debugShowTexture(World& world, unsigned int textureID,
                        uint32_t screenIndex);
  void debugShowShadowMapTexture(World& world, unsigned int textureID,
                                 uint32_t screenIndex, bool isPerspective,
                                 float farPlane, float nearPlane);
  std::unique_ptr<DeferredShadingRenderer> renderer_{nullptr};
  std::unique_ptr<SpriteRenderer> debugRenderer_{nullptr};
  std::unique_ptr<ShadowMapDebugRenderer> shapdowMapDebugRenderer_{nullptr};
};