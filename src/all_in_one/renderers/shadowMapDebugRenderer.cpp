#include "shadowMapDebugRenderer.h"

#include <glad/glad.h>

#include "util/check.h"
#include "util/toString.h"
ShadowMapDebugRenderer::ShadowMapDebugRenderer(const Shader &shader) {
  this->shader = shader;
  this->initRenderData();
}

ShadowMapDebugRenderer::~ShadowMapDebugRenderer() {
  glDeleteVertexArrays(1, &this->quadVAO);
}

void ShadowMapDebugRenderer::draw(float farPlane, float nearPlane,
                                  bool isPerspective, const Texture2D &texture,
                                  glm::vec2 position, glm::vec2 size,
                                  float rotate) {
  // prepare transformations
  this->shader.use();
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(
      model, glm::vec3(position,
                       0.0f));  // first translate (transformations are: scale
                                // happens first, then rotation, and then final
                                // translation happens; reversed order)

  model = glm::translate(
      model, glm::vec3(0.5f * size.x, 0.5f * size.y,
                       0.0f));  // move origin of rotation to center of quad
  model = glm::rotate(model, glm::radians(rotate),
                      glm::vec3(0.0f, 0.0f, 1.0f));  // then rotate
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y,
                                          0.0f));  // move origin back

  model = glm::scale(model, glm::vec3(size, 1.0f));  // last scale
  this->shader.setMatrix4("model", model);
  this->shader.setFloat("farPlane", farPlane);
  this->shader.setFloat("nearPlane", nearPlane);
  this->shader.setInteger("isPerspective", int(isPerspective));
  //   // render textured quad
  glActiveTexture(GL_TEXTURE0);
  texture.bind();

  glBindVertexArray(this->quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void ShadowMapDebugRenderer::initRenderData() {
  // configure VAO/VBO
  unsigned int VBO;
  // couner-clock-wise
  float vertices[] = {                          // pos      // tex
                      0.0f, 1.0f, 0.0f, 1.0f,   // left-top
                      0.0f, 0.0f, 0.0f, 0.0f,   // left-bottom
                      1.0f, 0.0f, 1.0f, 0.0f,   // right-bottom
                                                // pos      // tex
                      0.0f, 1.0f, 0.0f, 1.0f,   // left-top
                      1.0f, 0.0f, 1.0f, 0.0f,   // right-bottom
                      1.0f, 1.0f, 1.0f, 1.0f};  // right-top

  glGenVertexArrays(1, &this->quadVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(this->quadVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}