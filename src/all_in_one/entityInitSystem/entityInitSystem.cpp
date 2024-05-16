#include "entityInitSystem.h"

#include "commonComponents/color.h"
#include "commonComponents/debugScreenConfig.h"
#include "commonComponents/light.h"
#include "commonComponents/rotation.h"
#include "commonComponents/tags.h"
#include "commonComponents/transform3d.h"
#include "commonComponents/uvScale.h"
#include "resources/resourceManager.h"
#include "util/check.h"
#include "util/fs.hpp"
#include "util/random.h"
#include "window/windowSize.h"

void buildDebugScreenConfig(World &world) {
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  uint32_t debugScreenHight = 100;
  uint32_t debugScreenWidth = uint32_t(
      debugScreenHight * (float(windowSize->width_) / windowSize->height_));
  std::cout << debugScreenWidth << "\n";
  uint32_t debugScreenHightGap = 1;
  uint32_t debugScreenWidthGap = 1;
  uint32_t screenNum = 4;
  uint32_t startPositionX = 0;
  uint32_t startPositionY = 0;
  DebugScreenConfig debugScreenConfig;
  for (int i = 0; i < screenNum; i++) {
    DebugScreenConfig::DebugScreen debugScreen;
    debugScreen.position =
        glm::vec2{startPositionX + i * debugScreenWidth, startPositionY};
    debugScreen.size = glm::vec2{debugScreenWidth, debugScreenHight};
    debugScreenConfig.screens.push_back(debugScreen);
  }
  world.addResource<DebugScreenConfig>(debugScreenConfig);
}

glm::vec3 randomVec3() {
  return glm::vec3{myRandom(0.0f, 1.0f), myRandom(0.0f, 1.0f),
                   myRandom(0.0f, 1.0f)};
}

size_t randomIndex() {
  float v = myRandom();
  if (v <= 0.33333f) {
    return 0;
  }
  if (v <= 0.66666f) {
    return 1;
  }
  if (v <= 1.0f) {
    return 2;
  }
  return 0;
}
glm::vec3 randomSingleColor(size_t index) {
  float v = myRandom();
  auto r = glm::vec3{0.0f};
  r[index] = v;
  return r;
}

LightAttenuation randomLightAttennuation() {
  return LightAttenuation{myRandom(), myRandom(), myRandom()};
}
LightMaterial randomLightMaterial() {
  return LightMaterial{randomVec3(), randomVec3(), randomVec3()};
}
LightMaterial randomRGBLightMaterial(float scale) {
  auto index = randomIndex();
  return LightMaterial{randomSingleColor(index) * scale,
                       randomSingleColor(index) * scale,
                       randomSingleColor(index) * scale};
}
LightMaterial zeroLightMaterial() {
  return LightMaterial{glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{0.0f}};
}

glm::vec3 lightMaterialToColor(const LightMaterial &material) {
  return material.ambient_ + material.diffuse_ + material.specular_;
}

std::string getRandomMaterialName() {
  static std::vector<std::string> names{"wall", "gold", "grass", "plastic",
                                        "rusted_iron"};
  int index = int(myRandom() * names.size()) % names.size();
  return names[index];
}

void loadResources(World &world) {
  ResourceManager::loadAllSimpleMesh();
  {
    ResourceManager::loadMaterials("wall", "../../resources/textures/pbr/wall/",
                                   std::vector<std::string>{
                                       "albedo.png",
                                       "normal.png",
                                       "roughness.png",
                                       "metallic.png",
                                       "ao.png",
                                   });
    ResourceManager::loadMaterials("gold", "../../resources/textures/pbr/gold/",
                                   std::vector<std::string>{
                                       "albedo.png",
                                       "normal.png",
                                       "roughness.png",
                                       "metallic.png",
                                       "ao.png",
                                   });
    ResourceManager::loadMaterials("grass",
                                   "../../resources/textures/pbr/grass/",
                                   std::vector<std::string>{
                                       "albedo.png",
                                       "normal.png",
                                       "roughness.png",
                                       "metallic.png",
                                       "ao.png",
                                   });
    ResourceManager::loadMaterials("plastic",
                                   "../../resources/textures/pbr/plastic/",
                                   std::vector<std::string>{
                                       "albedo.png",
                                       "normal.png",
                                       "roughness.png",
                                       "metallic.png",
                                       "ao.png",
                                   });
    ResourceManager::loadMaterials("rusted_iron",
                                   "../../resources/textures/pbr/rusted_iron/",
                                   std::vector<std::string>{
                                       "albedo.png",
                                       "normal.png",
                                       "roughness.png",
                                       "metallic.png",
                                       "ao.png",
                                   });
    ResourceManager::loadMaterials("backpack_material",
                                   "../../resources/objects/backpack/",
                                   std::vector<std::string>{
                                       "diffuse.jpg",
                                       "normal.png",
                                       "roughness.jpg",
                                       "specular.jpg",
                                       "ao.jpg",
                                   });
  }
  {
    ResourceManager::loadModel(
        "backpack_mesh", "../../resources/objects/backpack/", "backpack.obj");
  }
  {
    std::string base{"../../resources/textures/skybox/"};
    std::vector<std::string> names{"right.jpg",  "left.jpg",  "top.jpg",
                                   "bottom.jpg", "front.jpg", "back.jpg"};
    std::vector<std::string> paths;
    for (int i = 0; i < names.size(); i++) {
      paths.push_back(base + names[i]);
    }
    ResourceManager::loadTextureCube(paths, false, "sky_sea");
  }
}
void loadBackpack(World &world) {
  auto backpack = EntityIDGenerator::alloc();
  world.addComponent<Transform3D>(backpack, Transform3D{});
  world.addComponent<Model>(backpack,
                            ResourceManager::getModel("backpack_mesh"));
  world.addComponent<Material>(
      backpack, ResourceManager::getMaterial("backpack_material"));
  world.addComponent<DeferredRendererTag>(backpack, DeferredRendererTag{});
  auto rotateMe = RotateMe{};
  rotateMe.aroundSelf = true;
  world.addComponent<RotateMe>(backpack, rotateMe);
  world.addComponent<UVScale>(backpack, UVScale{glm::vec2{1.0f, 1.0f}});
  // world.addComponent<SimpleMeshRendererTag>(backpack,
  // SimpleMeshRendererTag{});
  // world.addComponent<SimpleMeshRendererTag::WireframeRendererTag>(
  //     backpack, SimpleMeshRendererTag::WireframeRendererTag{});
}
void buildDirLight(World &world) {
  float xPos = myRandom() * 6.0f - 3.0f;
  float yPos = myRandom() * 6.0f - 4.0f;
  float zPos = myRandom() * 6.0f - 3.0f;
  auto randomPostion = glm::vec3{xPos, yPos, zPos};
  auto randomDirection = randomVec3();
  randomDirection[2] = -1.0f * std::abs(randomDirection[2]);
  // randomPostion = glm::vec3{10.0f, 0.0f, 0.0f};
  // randomDirection = glm::normalize(glm::vec3{-1.0f, 0.0f, 0.0f});
  EntityID dirLight = EntityIDGenerator::alloc();
  world.addComponent<Transform3D>(
      dirLight, Transform3D{randomPostion, glm::vec3{0.2f, 0.2f, 0.2f}});
  // auto material = zeroLightMaterial();
  auto material = randomLightMaterial();
  world.addComponent<DirLight>(dirLight, DirLight{randomDirection, material});
  // float rColor = myRandom(0.5f, 1.0f);
  // float gColor = myRandom(0.5f, 1.0f);
  // float bColor = myRandom(0.5f, 1.0f);
  world.addComponent<BaseColor>(
      dirLight,
      BaseColor{BaseColorType::FIXED, lightMaterialToColor(material)});
  world.addComponent<SimpleMesh>(dirLight,
                                 ResourceManager::getSimpleMesh("cube"));
  world.addComponent<SimpleMeshRendererTag>(dirLight, SimpleMeshRendererTag{});
  // world.addComponent<RotateMe>(dirLight,
  // RotateMe{glm::vec3{0.0f,1.0f,0.0f}});
}
void buildSpotLight(World &world) {
  EntityID spotLight = EntityIDGenerator::alloc();
  float xPos = myRandom() * 6.0f - 3.0f;
  float yPos = myRandom() * 6.0f - 4.0f;
  float zPos = myRandom() * 6.0f - 3.0f;
  world.addComponent<Transform3D>(
      spotLight,
      Transform3D{glm::vec3{xPos, yPos, zPos}, glm::vec3{1.0f, 0.2f, 0.2f}});
  // auto material = randomLightMaterial();
  // material.ambient_ *= 10.0f;
  auto material = zeroLightMaterial();
  world.addComponent<SpotLight>(spotLight,
                                SpotLight{0.0f - randomVec3(), 0.99f, 0.5f,
                                          randomLightAttennuation(), material});
  world.addComponent<BaseColor>(
      spotLight,
      BaseColor{BaseColorType::FIXED, lightMaterialToColor(material)});
  world.addComponent<SimpleMesh>(spotLight,
                                 ResourceManager::getSimpleMesh("cube"));
  world.addComponent<SimpleMeshRendererTag>(spotLight, SimpleMeshRendererTag{});
  world.addComponent<RotateMe>(spotLight, RotateMe{});
}
void buildPointLightObjects(World &world) {
  int pointLightNum{1};
  for (unsigned int i = 0; i < pointLightNum; i++) {
    EntityID pointLight = EntityIDGenerator::alloc();
    // calculate slightly random offsets
    float xPos = myRandom() * 6.0f - 3.0f;
    float yPos = myRandom() * 6.0f - 4.0f;
    float zPos = myRandom() * 6.0f - 3.0f;
    world.addComponent<Transform3D>(
        pointLight,
        Transform3D{glm::vec3{xPos, yPos, zPos}, glm::vec3{0.2f, 0.2f, 0.2f}});
    // also calculate random color
    // auto material = randomLightMaterial();
    // auto material = randomRGBLightMaterial(1.0f);
    auto material = zeroLightMaterial();
    world.addComponent<_PointLight>(
        pointLight, _PointLight{material, randomLightAttennuation()});
    world.addComponent<BaseColor>(
        pointLight,
        BaseColor{BaseColorType::FIXED, lightMaterialToColor(material)});
    world.addComponent<SimpleMesh>(pointLight,
                                   ResourceManager::getSimpleMesh("cube"));
    world.addComponent<SimpleMeshRendererTag>(pointLight,
                                              SimpleMeshRendererTag{});
    world.addComponent<SimpleMeshRendererTag::WireframeRendererTag>(
        pointLight, SimpleMeshRendererTag::WireframeRendererTag{});
    world.addComponent<RotateMe>(pointLight, RotateMe{});
  }
}
void loadWalls(World &world) {
  glm::vec3 color = glm::vec3{1.0f, 0.5f, 0.5f};
  glm::vec3 size{14, 14, 14};
  glm::vec3 basePosition = -size / 2.0f;
  // glm::vec3 basePosition{0.0f,0.0f,0.0f};
  float thickness{0.1};
  for (int i = 0; i < 3; i++) {
    auto scale = size;
    scale[i] = thickness;
    auto color = glm::vec3{0.0f, 0.0f, 0.0f};
    color[i] = 1.0f;
    EntityID wall = EntityIDGenerator::alloc();
    world.addComponent<Transform3D>(wall, Transform3D{basePosition, scale});
    world.addComponent<BaseColor>(wall, BaseColor{BaseColorType::FIXED, color});
    world.addComponent<SimpleMesh>(wall,
                                   ResourceManager::getSimpleMesh("cube"));
    world.addComponent<Material>(wall, ResourceManager::getMaterial("wall"));
    world.addComponent<UVScale>(wall, UVScale{glm::vec2{10.0f, 10.0f}});
    world.addComponent<DeferredRendererTag>(wall, DeferredRendererTag{});
    // world.addComponent<SimpleMeshRendererTag>(wall,
    //                                           SimpleMeshRendererTag{});
    // world.addComponent<SimpleMeshRendererTag::WireframeRendererTag>(wall,
    // SimpleMeshRendererTag::WireframeRendererTag{});
  }
}

void loadBricks_Bottom(World &world) {
  float step{2};
  float minx{-10};
  float miny{-10};
  float maxx{10};
  float maxy{10};
  for (float x = minx; x <= maxx; x += step) {
    for (float y = miny; y <= maxy; y += step) {
      EntityID brick = EntityIDGenerator::alloc();
      glm::vec3 position{x, -5.0 + 2.0f * myRandom(), y};
      glm::vec3 scale{step / 2, 1.0f, step / 2};
      world.addComponent<Transform3D>(brick, Transform3D{position, scale});
      world.addComponent<BaseColor>(
          brick, BaseColor{BaseColorType::FIXED, randomVec3()});
      world.addComponent<SimpleMesh>(brick,
                                     ResourceManager::getSimpleMesh("cube"));
      world.addComponent<Material>(
          brick, ResourceManager::getMaterial(getRandomMaterialName()));
      world.addComponent<UVScale>(brick, UVScale{glm::vec2{1.0f, 1.0f}});
      world.addComponent<DeferredRendererTag>(brick, DeferredRendererTag{});
    }
  }
}

void loadBricks_Top(World &world) {
  float step{2};
  float minx{-10};
  float miny{-10};
  float maxx{10};
  float maxy{10};
  float high{10.0f};
  for (float x = minx; x <= maxx; x += step) {
    for (float y = miny; y <= maxy; y += step) {
      EntityID brick = EntityIDGenerator::alloc();
      glm::vec3 position{x, -5.0 + 2.0f * myRandom() + high, y};
      glm::vec3 scale{step / 2, 1.0f, step / 2};
      world.addComponent<Transform3D>(brick, Transform3D{position, scale});
      world.addComponent<BaseColor>(
          brick, BaseColor{BaseColorType::FIXED, randomVec3()});
      world.addComponent<SimpleMesh>(brick,
                                     ResourceManager::getSimpleMesh("cube"));
      world.addComponent<Material>(
          brick, ResourceManager::getMaterial(getRandomMaterialName()));
      world.addComponent<UVScale>(brick, UVScale{glm::vec2{1.0f, 1.0f}});
      world.addComponent<DeferredRendererTag>(brick, DeferredRendererTag{});
    }
  }
}

void loadAxis(World &world) {
  glm::vec3 size{14, 14, 14};
  glm::vec3 basePosition{0.0f, 0.0f, 0.0f};
  float thickness{0.1};
  for (int i = 0; i < 3; i++) {
    glm::vec3 scale{thickness};
    scale[i] = size[i];
    auto color = glm::vec3{0.0f, 0.0f, 0.0f};
    color[i] = 1.0f;
    EntityID wall = EntityIDGenerator::alloc();
    world.addComponent<Transform3D>(wall, Transform3D{basePosition, scale});
    world.addComponent<BaseColor>(wall, BaseColor{BaseColorType::FIXED, color});
    world.addComponent<SimpleMesh>(wall,
                                   ResourceManager::getSimpleMesh("cube"));
    world.addComponent<SimpleMeshRendererTag>(wall, SimpleMeshRendererTag{});
    // world.addComponent<SimpleMeshRendererTag::WireframeRendererTag>(
    //     wall, SimpleMeshRendererTag::WireframeRendererTag{});
  }
}

void EntityInitSystem::start(World &world) {
  // auto windowSize = world.getResource<WindowSize>();
  // CHECK(windowSize != nullptr)
  world.addResource(MaterialConfig{8});
  buildDebugScreenConfig(world);
  loadResources(world);
  loadAxis(world);
  loadBackpack(world);
  buildDirLight(world);
  buildSpotLight(world);
  buildPointLightObjects(world);
  loadBricks_Bottom(world);
  loadBricks_Top(world);

  // {
  //   CHECK(fs::exist("../../resources/textures/background.jpg"))
  //   ResourceManager::loadTexture("../../resources/textures/background.jpg",
  //                                false, "background");
  //   auto backgroundEntityID = EntityIDGenerator::alloc();
  //   auto windowSize = world.getResource<WindowSize>();
  //   CHECK(windowSize != nullptr)
  //   world.addComponent(
  //       backgroundEntityID,
  //       Transform2D{glm::vec2{0.0f, 0.0f},
  //                   glm::vec2{windowSize->width_, windowSize->height_},
  //                   0.0});
  //   world.addComponent(backgroundEntityID,
  //                      SpritColor{glm::vec3{1.0f, 1.0f, 1.0f},
  //                                 ResourceManager::getTexture("background")});
  // }
}