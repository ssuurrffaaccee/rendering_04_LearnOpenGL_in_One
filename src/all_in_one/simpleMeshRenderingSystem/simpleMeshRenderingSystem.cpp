#include "simpleMeshRenderingSystem.h"

#include <glm/gtc/matrix_transform.hpp>

#include "cameraSystem/camera.h"
#include "commonComponents/color.h"
#include "commonComponents/light.h"
#include "commonComponents/tags.h"
#include "commonComponents/transform3d.h"
#include "resources/resourceManager.h"
#include "util/check.h"
#include "util/fs.hpp"
#include "util/toString.h"
#include "window/windowSize.h"
namespace {
void doRender0(SimpleMeshRenderer& renderer, Camera& camera,
               Transform3D& tranform, Mesh& mesh, int32_t windowWidth,
               int32_t windowHeights, const glm::vec3& baseColor,
               const SimpleMeshRenderPolygonMode& polygonMode) {
  auto model = tranform.getModelTranstorm();
  auto view = camera.getViewMatrix();
  glm::mat4 projection =
      glm::perspective(glm::radians(camera.zoom_),
                       (float)windowWidth / (float)windowHeights, 0.1f, 100.0f);
  // std::cout << toString(model) << toString(view) << toString(projection);
  // std::cout<<mesh.VAO_<<"===="<<mesh.indices_.size()<<"\n";
  //  std::cout << toString(baseColor) << int(polygonMode) <<"\n\n\n";
  renderer.draw(mesh.VAO_, mesh.indices_.size(), model, view, projection,
                baseColor, polygonMode);
}
void doRender1(SimpleMeshRenderer& renderer, Camera& camera,
               Transform3D& tranform, SimpleMesh& mesh, int32_t windowWidth,
               int32_t windowHeights, const glm::vec3& baseColor,
               const SimpleMeshRenderPolygonMode& polygonMode) {
  auto model = tranform.getModelTranstorm();
  auto view = camera.getViewMatrix();
  glm::mat4 projection =
      glm::perspective(glm::radians(camera.zoom_),
                       (float)windowWidth / (float)windowHeights, 0.1f, 100.0f);
  // std::cout << toString(model) << toString(view) << toString(projection);
  renderer.draw(mesh.VAO_, mesh.indexSize, model, view, projection, baseColor,
                polygonMode);
}
}  // namespace
SimpleMeshRenderingSystem::~SimpleMeshRenderingSystem() {}
void SimpleMeshRenderingSystem::start(World& world) {
  std::string vertSource{"simpleMesh.vert"};
  std::string fragSource{"simpleMesh.frag"};
  CHECK(fs::exist(vertSource))
  CHECK(fs::exist(fragSource))
  ResourceManager::loadShader(vertSource.c_str(), fragSource.c_str(), nullptr,
                              "simpleMesh");
  renderer_ = std::make_unique<SimpleMeshRenderer>(
      ResourceManager::getShader("simpleMesh"));
}
void SimpleMeshRenderingSystem::update(float dt, World& world) {}
void SimpleMeshRenderingSystem::render(World& world) {
  auto& simpleMeshRendererTagEntities =
      world.getEntities<SimpleMeshRendererTag>();
  auto* camera = world.getResource<Camera>();
  CHECK(camera != nullptr)
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  static glm::vec3 defaultColor{1.0f, 1.0f, 1.0f};
  glm::vec3 baseColor = defaultColor;

  for (auto& entityID : simpleMeshRendererTagEntities) {
    SimpleMeshRenderPolygonMode polygonMode = SimpleMeshRenderPolygonMode::FILL;
    auto* transfrom3DComponent = world.getComponent<Transform3D>(entityID);
    if (transfrom3DComponent == nullptr) {
      continue;
    }
    auto* baseColorComponent = world.getComponent<BaseColor>(entityID);
    if (baseColorComponent != nullptr) {
      baseColor = baseColorComponent->getColor();
    }
    auto* wireframeRendererTagComponent =
        world.getComponent<SimpleMeshRendererTag::WireframeRendererTag>(
            entityID);
    if (wireframeRendererTagComponent != nullptr) {
      polygonMode = SimpleMeshRenderPolygonMode::WIREFRAME;
    }
    auto* modelComponent = world.getComponent<Model>(entityID);
    if (modelComponent != nullptr) {
      for (auto& mesh : modelComponent->meshs_) {
        doRender0(*(renderer_.get()), *camera, *transfrom3DComponent, *mesh,
                  windowSize->width_, windowSize->height_, baseColor,
                  polygonMode);
      }
      continue;
    }
    auto* simpleMeshComponent = world.getComponent<SimpleMesh>(entityID);
    if (simpleMeshComponent != nullptr) {
      doRender1(*(renderer_.get()), *camera, *transfrom3DComponent,
                *simpleMeshComponent, windowSize->width_, windowSize->height_,
                baseColor, polygonMode);
      continue;
    }
  }
  // auto& transfrom3DEntities = world.getEntities<Transform3D>();
  // if (transfrom3DEntities.empty()) return;
  // auto& modelEntities = world.getEntities<Model>();
  // if (modelEntities.empty()) return;
  // auto* camera = world.getResource<Camera>();
  // CHECK(camera != nullptr)
  // auto windowSize = world.getResource<WindowSize>();
  // CHECK(windowSize != nullptr)
  // auto& minSizeEntities = min(transfrom3DEntities, modelEntities);
  // for (auto& entityID : minSizeEntities) {
  //   auto* transfrom3DComponent = world.getComponent<Transform3D>(entityID);
  //   auto* modelComponent = world.getComponent<Model>(entityID);
  //   if (transfrom3DComponent == nullptr || modelComponent == nullptr) {
  //     continue;
  //   }
  //   for (auto& mesh : modelComponent->meshs_) {
  //     doRender(*(renderer_.get()), *camera, *transfrom3DComponent, *mesh,
  //              windowSize->width_, windowSize->height_);
  //   }
  // }
}