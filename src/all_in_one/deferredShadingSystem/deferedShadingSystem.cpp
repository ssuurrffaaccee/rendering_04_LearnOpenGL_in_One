#include "cameraSystem/camera.h"
#include "commonComponents/color.h"
#include "commonComponents/debugScreenConfig.h"
#include "commonComponents/light.h"
#include "commonComponents/tags.h"
#include "commonComponents/transform3d.h"
#include "commonComponents/uvScale.h"
#include "deferredShadingSystem.h"
#include "resources/resourceManager.h"
#include "util/check.h"
#include "util/fs.hpp"
#include "util/random.h"
#include "util/toString.h"

namespace {
glm::mat4 calLightSpaceMatrix(const glm::vec3 &dirLightDirection,
                              const glm::vec3 &dirLightPosition) {
  GLfloat nearPlane = 0.1f, farPlane = 100.0f;
  glm::mat4 lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
  glm::mat4 lightView =
      glm::lookAt(dirLightPosition, dirLightPosition + dirLightDirection,
                  glm::vec3{0.0f, 1.0f, 0.0f});
  return lightProjection * lightView;
}
void doRenderDirShadowMap0(DeferredShadingRenderer &renderer, Camera &camera,
                           Transform3D &tranform, Mesh &mesh,
                           glm::vec3 &dirLightPosition,
                           glm::vec3 &dirLightDiretion) {
  auto model = tranform.getModelTranstorm();
  auto lightSpaceMatrix =
      calLightSpaceMatrix(dirLightDiretion, dirLightPosition);
  renderer.drawShadowMap(mesh.VAO_, mesh.indices_.size(), lightSpaceMatrix,
                         model);
}
void doRenderDirShadowMap1(DeferredShadingRenderer &renderer, Camera &camera,
                           Transform3D &tranform, SimpleMesh &simpleMesh,
                           glm::vec3 &dirLightPosition,
                           glm::vec3 &dirLightDiretion) {
  auto model = tranform.getModelTranstorm();
  auto lightSpaceMatrix =
      calLightSpaceMatrix(dirLightDiretion, dirLightPosition);
  renderer.drawShadowMap(simpleMesh.VAO_, simpleMesh.indexSize,
                         lightSpaceMatrix, model);
}
void doRenderGeometry0(DeferredShadingRenderer &renderer, Camera &camera,
                       Transform3D &tranform, Mesh &mesh,
                       const Material &material, int32_t windowWidth,
                       int32_t windowHeights, const glm::vec2 &uvScale) {
  auto model = tranform.getModelTranstorm();
  auto view = camera.getViewMatrix();
  glm::mat4 projection =
      glm::perspective(glm::radians(camera.zoom_),
                       (float)windowWidth / (float)windowHeights, 0.1f, 100.0f);
  // std::cout << toString(model) << toString(view) << toString(projection);
  renderer.drawGeometry(mesh.VAO_, mesh.indices_.size(), model, view,
                        projection, material.diffuse_, material.normal_,
                        material.specular_, uvScale);
}
void doRenderGeometry1(DeferredShadingRenderer &renderer, Camera &camera,
                       Transform3D &tranform, SimpleMesh &simpleMesh,
                       const Material &material, int32_t windowWidth,
                       int32_t windowHeights, const glm::vec2 &uvScale) {
  auto model = tranform.getModelTranstorm();
  auto view = camera.getViewMatrix();
  glm::mat4 projection =
      glm::perspective(glm::radians(camera.zoom_),
                       (float)windowWidth / (float)windowHeights, 0.1f, 100.0f);
  // std::cout << toString(model) << toString(view) << toString(projection);
  renderer.drawGeometry(simpleMesh.VAO_, simpleMesh.indexSize, model, view,
                        projection, material.diffuse_, material.normal_,
                        material.specular_, uvScale);
}
glm::vec3 rotationToDirection(glm::vec3 &rotation) {
  auto model = glm::mat4(1.0f);
  model = glm::rotate(model, rotation[2], glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::rotate(model, rotation[1], glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, rotation[0], glm::vec3(1.0f, 0.0f, 0.0f));
  return glm::vec4{1.0f, 0.0f, 0.0f, 1.0f} * model;
}
}  // namespace
void DeferredShadingSystem::start(World &world) {
  {
    std::string vertSource{"./sprite.vert"};
    std::string fragSource{"./sprite.frag"};
    CHECK(fs::exist(vertSource))
    CHECK(fs::exist(fragSource))
    ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                                "deferrd_debug");
  }
  {
    std::string vertSource{"./deferred_gbuffer.vert"};
    std::string fragSource{"./deferred_gbuffer.frag"};
    CHECK(fs::exist(vertSource))
    CHECK(fs::exist(fragSource))
    ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                                "deferred_gbuffer");
  }
  {
    std::string vertSource{"./deferred_shading_one.vert"};
    std::string fragSource{"./deferred_shading_one.frag"};
    CHECK(fs::exist(vertSource))
    CHECK(fs::exist(fragSource))
    ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                                "deferred_shading");
  }
  {
    std::string vertSource{"./shadow_mapping_depth.vert"};
    std::string fragSource{"./shadow_mapping_depth.frag"};
    CHECK(fs::exist(vertSource))
    CHECK(fs::exist(fragSource))
    ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                                "dir_shadowmap");
  }
  {
    std::string vertSource{"./shadow_mapping_debug_show.vert"};
    std::string fragSource{"./shadow_mapping_debug_show.frag"};
    CHECK(fs::exist(vertSource))
    CHECK(fs::exist(fragSource))
    ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                                "shadowmap_debug");
  }
  renderer_ = std::make_unique<DeferredShadingRenderer>(
      ResourceManager::getShader("deferred_gbuffer"),
      ResourceManager::getShader("deferred_shading"),
      ResourceManager::getShader("dir_shadowmap"));
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr);
  {
    glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(windowSize->width_), 0.0f,
                   static_cast<float>(windowSize->height_), -1.0f, 1.0f);
    debugRenderer_ = std::make_unique<SpriteRenderer>(
        ResourceManager::getShader("deferrd_debug"));
    ResourceManager::getShader("deferrd_debug")
        .use()
        .setMatrix4("projection", projection);
  }
  {
    glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(windowSize->width_), 0.0f,
                   static_cast<float>(windowSize->height_), -1.0f, 1.0f);
    shapdowMapDebugRenderer_ = std::make_unique<ShadowMapDebugRenderer>(
        ResourceManager::getShader("shadowmap_debug"));
    ResourceManager::getShader("shadowmap_debug")
        .use()
        .setMatrix4("projection", projection);
  }
  renderer_->initDeferredShadingFrameBuffer(windowSize->width_,
                                            windowSize->height_);
  renderer_->setGamma(2.2f);
  renderer_->setHDRExposure(1.0f);
  renderer_->initDirShadowMapFrameBuffer(windowSize->width_,
                                         windowSize->height_);
  // std::cout << renderer_->gAlbedoSpec_ << " " << renderer_->gNormal_ << " "
  //           << renderer_->gPosition_ << "\n";
}
void DeferredShadingSystem::update(float dt, World &world) {}
void DeferredShadingSystem::render(World &world) {
  auto *camera = world.getResource<Camera>();
  CHECK(camera != nullptr)
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr);
  // dir shadow_map
  {
    glm::vec3 dirLightPosition;
    glm::vec3 dirLightDiretion;
    {
      auto &dirLightEntities = world.getEntities<DirLight>();
      if (dirLightEntities.empty()) return;
      CHECK_WITH_INFO(dirLightEntities.size() == 1,
                      std::to_string(dirLightEntities.size()))
      for (auto &entityID : dirLightEntities) {
        auto *dirLightComponent = world.getComponent<DirLight>(entityID);
        if (dirLightComponent == nullptr) {
          continue;
        }
        auto *transfrom3DComponent = world.getComponent<Transform3D>(entityID);
        if (transfrom3DComponent == nullptr) {
          continue;
        }
        dirLightDiretion = dirLightComponent->direction_;
        dirLightPosition = transfrom3DComponent->position_;
      }
    }
    auto &deferredRendererTagEntities =
        world.getEntities<DeferredRendererTag>();
    if (deferredRendererTagEntities.empty()) return;
    renderer_->beginDirShadowMapFrameBuffer();
    for (auto &entityID : deferredRendererTagEntities) {
      auto *transfrom3DComponent = world.getComponent<Transform3D>(entityID);
      if (transfrom3DComponent == nullptr) {
        continue;
      }
      auto *modelComponent = world.getComponent<Model>(entityID);
      if (modelComponent != nullptr) {
        for (auto &mesh : modelComponent->meshs_) {
          doRenderDirShadowMap0(*(renderer_.get()), *camera,
                                *transfrom3DComponent, *mesh, dirLightPosition,
                                dirLightDiretion);
        }
        continue;
      }
      auto *simpleMeshComponent = world.getComponent<SimpleMesh>(entityID);
      if (simpleMeshComponent != nullptr) {
        doRenderDirShadowMap1(*(renderer_.get()), *camera,
                              *transfrom3DComponent, *simpleMeshComponent,
                              dirLightPosition, dirLightDiretion);
        continue;
      }
    }
    renderer_->endDirShadowMapFrameBuffer();
  }
  // geometry pass
  {
    auto &deferredRendererTagEntities =
        world.getEntities<DeferredRendererTag>();
    if (deferredRendererTagEntities.empty()) return;
    renderer_->beginDeferredShadingFrameBuffer();
    for (auto &entityID : deferredRendererTagEntities) {
      auto *transfrom3DComponent = world.getComponent<Transform3D>(entityID);
      if (transfrom3DComponent == nullptr) {
        continue;
      }
      auto *materialComponent = world.getComponent<Material>(entityID);
      if (materialComponent == nullptr) {
        continue;
      }
      auto uvScaleComponent = world.getComponent<UVScale>(entityID);
      if (uvScaleComponent == nullptr) {
        continue;
      }
      auto *modelComponent = world.getComponent<Model>(entityID);
      if (modelComponent != nullptr) {
        for (auto &mesh : modelComponent->meshs_) {
          doRenderGeometry0(*(renderer_.get()), *camera, *transfrom3DComponent,
                            *mesh, *materialComponent, windowSize->width_,
                            windowSize->height_, uvScaleComponent->scale_);
        }
        continue;
      }
      auto *simpleMeshComponent = world.getComponent<SimpleMesh>(entityID);
      if (simpleMeshComponent != nullptr && materialComponent != nullptr) {
        doRenderGeometry1(*(renderer_.get()), *camera, *transfrom3DComponent,
                          *simpleMeshComponent, *materialComponent,
                          windowSize->width_, windowSize->height_,
                          uvScaleComponent->scale_);
        continue;
      }
    }
    renderer_->endDeferredShadingFrameBuffer();
  }

  // light pass
  {
    // glm::vec3 dirLightPosition;
    DirLight *dirLight{nullptr};
    {
      auto &dirLightEntities = world.getEntities<DirLight>();
      if (dirLightEntities.empty()) return;
      CHECK_WITH_INFO(dirLightEntities.size() == 1,
                      std::to_string(dirLightEntities.size()))
      for (auto &entityID : dirLightEntities) {
        auto *dirLightComponent = world.getComponent<DirLight>(entityID);
        if (dirLightComponent == nullptr) {
          continue;
        }
        dirLight = dirLightComponent;
      }
    }
    glm::vec3 spotLightPosition;
    SpotLight *spotLight{nullptr};
    {
      auto &spotLightEntities = world.getEntities<SpotLight>();
      if (spotLightEntities.empty()) return;
      CHECK_WITH_INFO(spotLightEntities.size() == 1,
                      std::to_string(spotLightEntities.size()))
      for (auto &entityID : spotLightEntities) {
        auto *spotLightComponent = world.getComponent<SpotLight>(entityID);
        auto *transfrom3DComponent = world.getComponent<Transform3D>(entityID);
        if (spotLightComponent == nullptr || transfrom3DComponent == nullptr) {
          continue;
        }
        spotLight = spotLightComponent;
        spotLight->direction =
            0.0f - rotationToDirection(transfrom3DComponent->rotation_);
        spotLightPosition = transfrom3DComponent->position_;
      }
    }
    std::vector<glm::vec3> pointLightPositions;
    std::vector<_PointLight *> pointLights;
    {
      auto &pointLightEntities = world.getEntities<_PointLight>();
      if (pointLightEntities.empty()) return;
      // CHECK_WITH_INFO(pointLightEntities.size() == MAX_POINT_LGITH_NUM,
      //                 std::to_string(MAX_POINT_LGITH_NUM))
      for (auto &entityID : pointLightEntities) {
        auto *transfrom3DComponent = world.getComponent<Transform3D>(entityID);
        auto *pointLightComponent = world.getComponent<_PointLight>(entityID);
        if (transfrom3DComponent == nullptr || pointLightComponent == nullptr) {
          continue;
        }
        pointLightPositions.push_back(transfrom3DComponent->position_);
        pointLights.push_back(pointLightComponent);
      }
    }
    CHECK(pointLightPositions.size() == 1)
    CHECK(pointLights.size() == 1)
    auto materialConfig = world.getResource<MaterialConfig>();
    renderer_->drawLights(camera->position_, *dirLight, spotLightPosition,
                          *spotLight, pointLightPositions[0], pointLights[0],
                          *materialConfig);
  }
  renderer_->copyDepthBuffer(windowSize->width_, windowSize->height_);
  debugShowTexture(world, renderer_->gNormal_, 0);
  debugShowTexture(world, renderer_->gAlbedoSpec_, 1);
  debugShowTexture(world, renderer_->gPosition_, 2);
  debugShowShadowMapTexture(world, renderer_->shadowMap_, 3, false, 0.0f, 0.0f);
}
void DeferredShadingSystem::debugShowTexture(World &world,
                                             unsigned int textureID,
                                             uint32_t screenIndex) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto debugScreenconfig = world.getResource<DebugScreenConfig>();
  CHECK(debugScreenconfig != nullptr)
  CHECK(screenIndex < debugScreenconfig->screens.size());
  auto &screen = debugScreenconfig->screens[screenIndex];
  Texture2D positionTexture;
  positionTexture.ID = textureID;
  debugRenderer_->drawSprite(positionTexture, screen.position, screen.size);
  // glm::vec2 size{static_cast<float>(windowSize.width_),
  //                static_cast<float>(windowSize.height_)};
  // int w = 10;
  // int h = 10;
  // std::vector<float> data;
  // data.resize(w*h*4);
  // glReadPixels(windowSize->width_ / 2, windowSize->height_ / 2, w, h,
  // GL_RGBA,
  //              GL_FLOAT, data.data());
  // for(auto& d: data){
  //   std::cout<<d<<"\n";
  // }
  // int w = 100;
  // int h = 100;
  // std::vector<float> data;
  // data.resize(w * h * 4, 0.5);
  // glBindTexture(GL_TEXTURE_2D, positionRexture.ID);
  // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 100, 100,
  //                 GL_RGBA, GL_FLOAT, data.data());
}
void DeferredShadingSystem::debugShowShadowMapTexture(
    World &world, unsigned int textureID, uint32_t screenIndex,
    bool isPerspective, float farPlane, float nearPlane) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto debugScreenconfig = world.getResource<DebugScreenConfig>();
  CHECK(debugScreenconfig != nullptr)
  CHECK(screenIndex < debugScreenconfig->screens.size());
  auto &screen = debugScreenconfig->screens[screenIndex];
  Texture2D positionTexture;
  positionTexture.ID = textureID;
  shapdowMapDebugRenderer_->draw(0.1f, 100.0f, isPerspective, positionTexture,
                                 screen.position, screen.size);
}