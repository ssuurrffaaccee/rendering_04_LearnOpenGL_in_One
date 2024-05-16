#include "deferredShadingRenderer.h"

#include <glad/glad.h>

#include "util/check.h"
#include "util/toString.h"
DeferredShadingRenderer::DeferredShadingRenderer(
    const Shader& geometryShader, const Shader& lightShader,
    const Shader& shadowMapShader) {
  geometryShader_ = geometryShader;
  lightShader_ = lightShader;
  shadowMapShader_ = shadowMapShader;
  initRenderData();
}
DeferredShadingRenderer::~DeferredShadingRenderer() {}
void DeferredShadingRenderer::initDeferredShadingFrameBuffer(
    uint32_t windowWidth, uint32_t windowHeight) {
  glGenFramebuffers(1, &gBuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer_);
  // position color buffer
  glGenTextures(1, &gPosition_);
  glBindTexture(GL_TEXTURE_2D, gPosition_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0,
               GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition_, 0);
  // normal color buffer
  glGenTextures(1, &gNormal_);
  glBindTexture(GL_TEXTURE_2D, gNormal_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0,
               GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal_, 0);
  // color + specular color buffer
  glGenTextures(1, &gAlbedoSpec_);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoSpec_, 0);
  unsigned int attachments_[3]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                               GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments_);
  // create and attach depth buffer (renderbuffer)
  glGenRenderbuffers(1, &rboDepth_);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth,
                        windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth_);
  // finally check if framebuffer is complete
  CHECK_WITH_INFO(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
      "Framebuffer not complete!")
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void DeferredShadingRenderer::beginDeferredShadingFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer_);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
}
void DeferredShadingRenderer::endDeferredShadingFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_CULL_FACE);
}
void DeferredShadingRenderer::drawGeometry(
    unsigned geometryVAO, unsigned indexSize, const glm::mat4& worldTransform,
    const glm::mat4& veiwTranform, const glm::mat4& projectionTransform,
    const Texture2D& diffuse, const Texture2D& normal,
    const Texture2D& specular, const glm::vec2& uvScale) {
  // std::cout << geometryVAO << " " << indexSize << " "
  //           << toString(worldTransform) << toString(veiwTranform)
  //           << toString(projectionTransform);
  this->geometryShader_.use();
  this->geometryShader_.setMatrix4("model", worldTransform);
  this->geometryShader_.setMatrix4("view", veiwTranform);
  this->geometryShader_.setMatrix4("projection", projectionTransform);
  this->geometryShader_.setVector2f("uv_scale", uvScale);
  glActiveTexture(GL_TEXTURE0);
  geometryShader_.setInteger("texture_diffuse", 0);
  glBindTexture(GL_TEXTURE_2D, diffuse.ID);
  glActiveTexture(GL_TEXTURE1);
  geometryShader_.setInteger("texture_specular", 1);
  glBindTexture(GL_TEXTURE_2D, specular.ID);
  glActiveTexture(GL_TEXTURE2);
  geometryShader_.setInteger("texture_normal", 2);
  glBindTexture(GL_TEXTURE_2D, normal.ID);
  glBindVertexArray(geometryVAO);
  glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
void DeferredShadingRenderer::drawLights(
    const glm::vec3& viewPos, const std::vector<glm::vec3>& lightPositions,
    const std::vector<glm::vec3>& lightColors) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  lightShader_.use();
  glActiveTexture(GL_TEXTURE0);
  lightShader_.setInteger("gPosition", 0);
  glBindTexture(GL_TEXTURE_2D, gPosition_);
  glActiveTexture(GL_TEXTURE1);
  lightShader_.setInteger("gNormal", 1);
  glBindTexture(GL_TEXTURE_2D, gNormal_);
  glActiveTexture(GL_TEXTURE2);
  lightShader_.setInteger("gAlbedoSpec", 2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec_);
  for (unsigned int i = 0; i < lightPositions.size(); i++) {
    lightShader_.setVector3f("lights[" + std::to_string(i) + "].Position",
                             lightPositions[i]);
    lightShader_.setVector3f("lights[" + std::to_string(i) + "].Color",
                             lightColors[i]);
    // update attenuation parameters and calculate radius
    const float linear = 0.7f;
    const float quadratic = 1.8f;
    lightShader_.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
    lightShader_.setFloat("lights[" + std::to_string(i) + "].Quadratic",
                          quadratic);
  }
  lightShader_.setVector3f("viewPos", viewPos);
  // send light relevant uniforms
  glBindVertexArray(this->quadVAO_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
void DeferredShadingRenderer::drawLights(
    const glm::vec3& viewPos, const DirLight& dirLight,
    const glm::vec3& spotLightPostiion, const SpotLight& spotLight,
    const std::vector<glm::vec3>& pointLightPosition,
    const std::vector<_PointLight*>& pointLights,
    const MaterialConfig& materialConfig) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  lightShader_.use();
  glActiveTexture(GL_TEXTURE0);
  lightShader_.setInteger("gbuffer.gPosition", 0);
  glBindTexture(GL_TEXTURE_2D, gPosition_);
  glActiveTexture(GL_TEXTURE1);
  lightShader_.setInteger("gbuffer.gNormal", 1);
  glBindTexture(GL_TEXTURE_2D, gNormal_);
  glActiveTexture(GL_TEXTURE2);
  lightShader_.setInteger("gbuffer.gAlbedoSpec", 2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec_);
  {
    lightShader_.setVector3f("dirLight.direction", dirLight.direction_);
    lightShader_.setVector3f("dirLight.ambient", dirLight.material_.ambient_);
    lightShader_.setVector3f("dirLight.diffuse", dirLight.material_.diffuse_);
    lightShader_.setVector3f("dirLight.specular", dirLight.material_.specular_);
  }
  {
    lightShader_.setVector3f("spotLight.position", spotLightPostiion);
    lightShader_.setVector3f("spotLight.direction", spotLight.direction);
    lightShader_.setFloat("spotLight.cutOff", spotLight.cutOff);
    lightShader_.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
    lightShader_.setVector3f("spotLight.ambient", spotLight.material_.ambient_);
    lightShader_.setVector3f("spotLight.diffuse", spotLight.material_.diffuse_);
    lightShader_.setVector3f("spotLight.specular",
                             spotLight.material_.specular_);
    lightShader_.setFloat("spotLight.constant",
                          spotLight.attenuation_.constant_);
    lightShader_.setFloat("spotLight.linear", spotLight.attenuation_.linear_);
    lightShader_.setFloat("spotLight.quadratic",
                          spotLight.attenuation_.quadratic_);
  }
  for (unsigned int i = 0; i < pointLights.size(); i++) {
    lightShader_.setVector3f("pointLights[" + std::to_string(i) + "].position",
                             pointLightPosition[i]);
    lightShader_.setVector3f("pointLights[" + std::to_string(i) + "].ambient",
                             pointLights[i]->material_.ambient_);
    lightShader_.setVector3f("pointLights[" + std::to_string(i) + "].diffuse",
                             pointLights[i]->material_.diffuse_);
    lightShader_.setVector3f("pointLights[" + std::to_string(i) + "].specular",
                             pointLights[i]->material_.specular_);
    // update attenuation parameters and calculate radius
    // const float linear = 0.7f;
    // const float quadratic = 1.8f;
    lightShader_.setFloat("pointLights[" + std::to_string(i) + "].constant",
                          pointLights[i]->attenuation_.constant_);
    lightShader_.setFloat("pointLights[" + std::to_string(i) + "].linear",
                          pointLights[i]->attenuation_.linear_);
    lightShader_.setFloat("pointLights[" + std::to_string(i) + "].quadratic",
                          pointLights[i]->attenuation_.quadratic_);
  }
  lightShader_.setVector3f("viewPos", viewPos);
  lightShader_.setFloat("materailConfig.shininess", materialConfig.shininess);
  // send light relevant uniforms
  glBindVertexArray(this->quadVAO_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
void DeferredShadingRenderer::drawLights(const glm::vec3& viewPos,
                                         const DirLight& dirLight,
                                         const glm::vec3& spotLightPostiion,
                                         const SpotLight& spotLight,
                                         const glm::vec3& pointLightPosition,
                                         _PointLight* pointLight,
                                         const MaterialConfig& materialConfig) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  lightShader_.use();
  glActiveTexture(GL_TEXTURE0);
  lightShader_.setInteger("gbuffer.gPosition", 0);
  glBindTexture(GL_TEXTURE_2D, gPosition_);
  glActiveTexture(GL_TEXTURE1);
  lightShader_.setInteger("gbuffer.gNormal", 1);
  glBindTexture(GL_TEXTURE_2D, gNormal_);
  glActiveTexture(GL_TEXTURE2);
  lightShader_.setInteger("gbuffer.gAlbedoSpec", 2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec_);
  {
    lightShader_.setVector3f("dirLight.direction", dirLight.direction_);
    lightShader_.setVector3f("dirLight.ambient", dirLight.material_.ambient_);
    lightShader_.setVector3f("dirLight.diffuse", dirLight.material_.diffuse_);
    lightShader_.setVector3f("dirLight.specular", dirLight.material_.specular_);
  }
  {
    lightShader_.setVector3f("spotLight.position", spotLightPostiion);
    lightShader_.setVector3f("spotLight.direction", spotLight.direction);
    lightShader_.setFloat("spotLight.cutOff", spotLight.cutOff);
    lightShader_.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
    lightShader_.setVector3f("spotLight.ambient", spotLight.material_.ambient_);
    lightShader_.setVector3f("spotLight.diffuse", spotLight.material_.diffuse_);
    lightShader_.setVector3f("spotLight.specular",
                             spotLight.material_.specular_);
    lightShader_.setFloat("spotLight.constant",
                          spotLight.attenuation_.constant_);
    lightShader_.setFloat("spotLight.linear", spotLight.attenuation_.linear_);
    lightShader_.setFloat("spotLight.quadratic",
                          spotLight.attenuation_.quadratic_);
  }
  {
    lightShader_.setVector3f("pointLight.position", pointLightPosition);
    lightShader_.setVector3f("pointLight.ambient",
                             pointLight->material_.ambient_);
    lightShader_.setVector3f("pointLight.diffuse",
                             pointLight->material_.diffuse_);
    lightShader_.setVector3f("pointLight.specular",
                             pointLight->material_.specular_);
    // update attenuation parameters and calculate radius
    // const float linear = 0.7f;
    // const float quadratic = 1.8f;
    lightShader_.setFloat("pointLight.constant",
                          pointLight->attenuation_.constant_);
    lightShader_.setFloat("pointLight.linear",
                          pointLight->attenuation_.linear_);
    lightShader_.setFloat("pointLight.quadratic",
                          pointLight->attenuation_.quadratic_);
    lightShader_.setVector3f("viewPos", viewPos);
    lightShader_.setFloat("materailConfig.shininess", materialConfig.shininess);
  }
  // send light relevant uniforms
  glBindVertexArray(this->quadVAO_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
void DeferredShadingRenderer::copyDepthBuffer(uint32_t windowWidth,
                                              uint32_t windowHeight) {
  unsigned srcFrameBuffer{gBuffer_};
  unsigned dstFrameBuffer{0};
  glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                    dstFrameBuffer);  // write to default framebuffer
  // blit to default framebuffer. Note that this may or may not work as the
  // internal formats of both the FBO and default framebuffer have to match.
  // the internal formats are implementation defined. This works on all of my
  // systems, but if it doesn't on yours you'll likely have to write to the
  // depth buffer in another shader stage (or somehow see to match the default
  // framebuffer's internal format with the FBO's internal format).
  glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth,
                    windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void DeferredShadingRenderer::initRenderData() {
  // configure VAO/VBO
  unsigned int VBO;
  // counter-clock-wise
  float vertices[] = {                           // pos      // tex
                      -1.0f, 1.0f, 0.0f, 1.0f,   // left-top
                      -1.0f, -1.0f, 0.0f, 0.0f,  // left-bottom
                      1.0f, -1.0f, 1.0f, 0.0f,   // right-bottom

                      // pos      // tex
                      -1.0f, 1.0f, 0.0f, 1.0f,  // left-top
                      1.0f, -1.0f, 1.0f, 0.0f,  // right-bottom
                      1.0f, 1.0f, 1.0f, 1.0f};  // right-top

  glGenVertexArrays(1, &this->quadVAO_);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(this->quadVAO_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void DeferredShadingRenderer::setGamma(float gamma) {
  lightShader_.use();
  lightShader_.setFloat("gamma", gamma);
}
void DeferredShadingRenderer::setHDRExposure(float exposure) {
  lightShader_.use();
  lightShader_.setFloat("exposure", exposure);
}

void DeferredShadingRenderer::drawShadowMap(unsigned geometryVAO,
                                            unsigned indexSize,
                                            const glm::mat4& lightSpaceTansform,
                                            const glm::mat4& worldTransform) {
  shadowMapShader_.use();
  shadowMapShader_.setMatrix4("model", worldTransform);
  shadowMapShader_.setMatrix4("lightSpaceMatrix", lightSpaceTansform);
  glBindVertexArray(geometryVAO);
  glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
void DeferredShadingRenderer::initDirShadowMapFrameBuffer(
    uint32_t windowWidth, uint32_t windowHeight) {
  glGenFramebuffers(1, &shadowMapBuffer_);
  // create depth texture
  glGenTextures(1, &shadowMap_);
  glBindTexture(GL_TEXTURE_2D, shadowMap_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadowMap_, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void DeferredShadingRenderer::beginDirShadowMapFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer_);
  glClear(GL_DEPTH_BUFFER_BIT);
}
void DeferredShadingRenderer::endDirShadowMapFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}