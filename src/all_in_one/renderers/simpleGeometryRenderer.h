#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources/shader.h"
enum class SimpleGeometryRendererPrimiteType { UNKOWN, TRIANGLE, LINE };
class SimpleGeometryRenderer {
 public:
  SimpleGeometryRenderer() = default;
  SimpleGeometryRenderer(const Shader& shader);
  ~SimpleGeometryRenderer();
  void drawGeometry(SimpleGeometryRendererPrimiteType primitiveType,
                    unsigned geometryVAO, unsigned indexSize,
                    glm::vec2 position,
                    glm::vec2 size = glm::vec2(10.0f, 10.0f),
                    float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

 private:
  Shader shader;
};