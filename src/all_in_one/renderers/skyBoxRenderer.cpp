#include "skyBoxRenderer.h"

#include <glad/glad.h>

#include "util/check.h"
SkyBoxRenderer::SkyBoxRenderer(const Shader& shader) { this->shader = shader; }
void SkyBoxRenderer::draw(unsigned geometryVAO, unsigned indexSize,
                          const glm::mat4& veiwTranform,
                          const glm::mat4& projectionTransform,
                          uint32_t cubeMap) {
  glm::mat4 noTransforVeiwTransform = glm::mat4(glm::mat3(veiwTranform));
  // prepare transformations
  this->shader.use();
  this->shader.setMatrix4("view", noTransforVeiwTransform);
  this->shader.setMatrix4("projection", projectionTransform);
  shader.use();
  // glDisable(GL_CULL_FACE);
  glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when
                           // values are equal to depth buffer's content
  this->shader.setInteger("skybox", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
  glBindVertexArray(geometryVAO);
  glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
  // glEnable(GL_CULL_FACE);
}