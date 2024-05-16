#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources/shader.h"
#include "resources/texture.h"
enum class SimpleMeshRenderPolygonMode { UNKONW, WIREFRAME, FILL };
class SimpleMeshRenderer {
 public:
  SimpleMeshRenderer() = default;
  SimpleMeshRenderer(const Shader& shader);
  ~SimpleMeshRenderer(){};
  void draw(unsigned geometryVAO, unsigned indexSize,
            const glm::mat4& worldTransform, const glm::mat4& veiwTranform,
            const glm::mat4& projectionTransform, const glm::vec3& baseColor,
            const SimpleMeshRenderPolygonMode& polygonMode);

 private:
  Shader shader;
};