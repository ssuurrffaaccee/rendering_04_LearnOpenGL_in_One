#include "skyBoxRenderingSystem.h"

#include <glm/gtc/matrix_transform.hpp>

#include "cameraSystem/camera.h"
// #include "commonComponents/color.h"
// #include "commonComponents/light.h"
// #include "commonComponents/tags.h"
// #include "commonComponents/transform3d.h"
#include "resources/resourceManager.h"
#include "util/check.h"
#include "util/fs.hpp"
#include "util/toString.h"
#include "window/windowSize.h"
// #include "window/windowSize.h"
SkyBoxRenderingSystem::~SkyBoxRenderingSystem() {}
void SkyBoxRenderingSystem::start(World& world) {
  std::string vertSource{"skybox.vert"};
  std::string fragSource{"skybox.frag"};
  CHECK(fs::exist(vertSource))
  CHECK(fs::exist(fragSource))
  ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                              "skybox");
  renderer_ =
      std::make_unique<SkyBoxRenderer>(ResourceManager::getShader("skybox"));
  auto skybox = EntityIDGenerator::alloc();
  world.addComponent<SimpleMesh>(skybox,
                                 ResourceManager::getSimpleMesh("cube"));
  world.addComponent<TextureCube>(skybox,
                                  ResourceManager::getTextureCube("sky_sea"));
  world.addComponent<SkyBoxTag>(skybox, SkyBoxTag{});
}
void SkyBoxRenderingSystem::update(float dt, World& world) {}
void SkyBoxRenderingSystem::render(World& world) {
  auto camera = world.getResource<Camera>();
  CHECK(camera != nullptr);
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  auto skyboxs = world.getEntities<SkyBoxTag>();
  CHECK(skyboxs.size() == 1)
  auto skyboxId = *skyboxs.begin();
  auto cubeMesh = world.getComponent<SimpleMesh>(skyboxId);
  auto cubeTexture = world.getComponent<TextureCube>(skyboxId);
  glm::mat4 projection = glm::perspective(
      glm::radians(camera->zoom_),
      (float)windowSize->width_ / (float)windowSize->height_, 0.1f, 100.0f);
  renderer_->draw(cubeMesh->VAO_, cubeMesh->indexSize, camera->getViewMatrix(),
                  projection, cubeTexture->ID);
}