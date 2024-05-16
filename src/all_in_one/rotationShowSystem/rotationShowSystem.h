#pragma once
#include "ecs/system.h"
#include "ecs/world.h"
#include "util/check.h"

struct RotationShowSystem : public System {
  RotationShowSystem() {}
  virtual ~RotationShowSystem() {}
  void start(World& world) override;
  void update(float dt, World& world) override;
};