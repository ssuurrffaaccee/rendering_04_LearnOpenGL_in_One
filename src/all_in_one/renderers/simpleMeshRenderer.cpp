#include "simpleMeshRenderer.h"

#include <glad/glad.h>

#include "util/check.h"
SimpleMeshRenderer::SimpleMeshRenderer(const Shader &shader) {
  this->shader = shader;
}
void SimpleMeshRenderer::draw(unsigned geometryVAO, unsigned indexSize,
                              const glm::mat4 &worldTransform,
                              const glm::mat4 &veiwTranform,
                              const glm::mat4 &projectionTransform,
                              const glm::vec3 &baseColor,
                              const SimpleMeshRenderPolygonMode &polygonMode) {
  // prepare transformations
  this->shader.use();
  this->shader.setMatrix4("WorldTransform", worldTransform);
  this->shader.setMatrix4("ViewTransform", veiwTranform);
  this->shader.setMatrix4("ProjectionTransform", projectionTransform);
  // render textured quad
  this->shader.setVector3f("baseColor", baseColor);
  glBindVertexArray(geometryVAO);
  if (polygonMode == SimpleMeshRenderPolygonMode::WIREFRAME) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if (polygonMode == SimpleMeshRenderPolygonMode::FILL) {
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
  } else {
    CHECK_WITH_INFO(false, "unkown polygon mode")
  }
  glBindVertexArray(0);
}