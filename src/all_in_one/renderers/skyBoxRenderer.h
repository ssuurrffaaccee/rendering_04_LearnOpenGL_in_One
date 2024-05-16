#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources/shader.h"
#include "resources/texture.h"
class SkyBoxRenderer {
 public:
  SkyBoxRenderer() = default;
  SkyBoxRenderer(const Shader& shader);
  ~SkyBoxRenderer(){};
  void draw(unsigned geometryVAO, unsigned indexSize, const glm::mat4& veiwTranform,
            const glm::mat4& projectionTransform,uint32_t cubeMap);

 private:
  Shader shader;
};