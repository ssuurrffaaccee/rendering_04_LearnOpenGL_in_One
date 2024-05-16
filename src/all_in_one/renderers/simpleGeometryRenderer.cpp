#include "simpleGeometryRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "util/check.h"
SimpleGeometryRenderer::SimpleGeometryRenderer(const Shader& shader) {
  this->shader = shader;
}
SimpleGeometryRenderer::~SimpleGeometryRenderer() {}
void SimpleGeometryRenderer::drawGeometry(
    SimpleGeometryRendererPrimiteType primitiveType, unsigned geometryVAO,
    unsigned indexSize, glm::vec2 position, glm::vec2 size, float rotate,
    glm::vec3 color) {
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
  // model = glm::rotate(model, glm::radians(rotate),
  //                     glm::vec3(0.0f, 0.0f, 1.0f));  // then rotate
  model =
      glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));  // then rotate
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y,
                                          0.0f));  // move origin back

  model = glm::scale(model, glm::vec3(size, 1.0f));  // last scale
  this->shader.setMatrix4("model", model);

  // render textured quad
  this->shader.setVector3f("baseColor", color);

  glBindVertexArray(geometryVAO);
  if (primitiveType == SimpleGeometryRendererPrimiteType::TRIANGLE) {
    glDrawArrays(GL_TRIANGLES, 0, indexSize);
  } else if (primitiveType == SimpleGeometryRendererPrimiteType::LINE) {
    glDrawArrays(GL_LINES, 0, indexSize);
  } else {
    CHECK_WITH_INFO(false, "unkown primtive type");
  }
  glBindVertexArray(0);
}