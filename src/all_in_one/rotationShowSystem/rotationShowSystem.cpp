#include "rotationShowSystem.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "commonComponents/rotation.h"
#include "commonComponents/tags.h"
#include "commonComponents/transform3d.h"
#include "util/toString.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
namespace {
glm::vec3 rotate(const glm::vec3 &position, const glm::vec3 &origin,
                 const glm::vec3 &axis, float speed, float dt) {
  auto relativePos = position - origin;
  glm::mat4 model{1.0f};
  model = glm::rotate(model, speed * dt, axis);
  relativePos = glm::vec4{relativePos, 1.0f} * model;
  return origin + relativePos;
}
}  // namespace
void RotationShowSystem::start(World &world) {}
void RotationShowSystem::update(float dt, World &world) {
  auto &rotateMeDEntities = world.getEntities<RotateMe>();
  if (rotateMeDEntities.empty()) return;
  for (auto &entityID : rotateMeDEntities) {
    auto *transfrom3DComponent = world.getComponent<Transform3D>(entityID);
    if (transfrom3DComponent == nullptr) {
      continue;
    }
    auto *rotateMeComponent = world.getComponent<RotateMe>(entityID);
    if (rotateMeComponent == nullptr) {
      continue;
    }
    transfrom3DComponent->position_ =
        rotate(transfrom3DComponent->position_, rotateMeComponent->origin,
               rotateMeComponent->axis, rotateMeComponent->speed, dt);
    if (rotateMeComponent->aroundSelf) {
      float ratationY = transfrom3DComponent->rotation_[1];
      ratationY =
          std::fmod(ratationY + rotateMeComponent->speed * dt, M_PI * 2.0f);
      transfrom3DComponent->rotation_[1] = ratationY;
    }
  }
}