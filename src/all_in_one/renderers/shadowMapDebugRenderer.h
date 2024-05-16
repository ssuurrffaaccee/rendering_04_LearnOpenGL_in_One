#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources/shader.h"
#include "resources/texture.h"

class ShadowMapDebugRenderer {
 public:
  ShadowMapDebugRenderer() = default;
  ShadowMapDebugRenderer(const Shader &shader);
  ~ShadowMapDebugRenderer();
  void draw(float farPlane, float nearPlane, bool isPerspective,
            const Texture2D &texture, glm::vec2 position,
            glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f);

 private:
  Shader shader;
  unsigned int quadVAO;
  void initRenderData();
};