#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "commonComponents/light.h"
#include "resources/shader.h"
#include "resources/texture.h"
class DeferredShadingRenderer {
 public:
  DeferredShadingRenderer() = default;
  DeferredShadingRenderer(const Shader& geometryShader,
                          const Shader& lightShader,
                          const Shader& shadowMapShader);
  ~DeferredShadingRenderer();
  void initDeferredShadingFrameBuffer(uint32_t windowWidth,
                                      uint32_t windowHeight);
  void beginDeferredShadingFrameBuffer();
  void endDeferredShadingFrameBuffer();
  void initDirShadowMapFrameBuffer(uint32_t windowWidth, uint32_t windowHeight);
  void beginDirShadowMapFrameBuffer();
  void endDirShadowMapFrameBuffer();
  void setGamma(float gamma);
  void setHDRExposure(float exposure);
  void drawShadowMap(unsigned geometryVAO, unsigned indexSize,
                     const glm::mat4& lightSpaceTansform,
                     const glm::mat4& worldTransform);
  void drawGeometry(unsigned geometryVAO, unsigned indexSize,
                    const glm::mat4& worldTransform,
                    const glm::mat4& veiwTranform,
                    const glm::mat4& projectionTransform,
                    const Texture2D& diffuse, const Texture2D& normal,
                    const Texture2D& specular, const glm::vec2& uvScale);
  void drawLights(const glm::vec3& viewPos,
                  const std::vector<glm::vec3>& lightPositions,
                  const std::vector<glm::vec3>& lightColors);
  void drawLights(const glm::vec3& viewPos, const DirLight& dirLight,
                  const glm::vec3& spotLightPostiion,
                  const SpotLight& spotLight,
                  const std::vector<glm::vec3>& pointLightPosition,
                  const std::vector<_PointLight*>& pointLights,
                  const MaterialConfig& materialConfig);
  void drawLights(const glm::vec3& viewPos, const DirLight& dirLight,
                  const glm::vec3& spotLightPostiion,
                  const SpotLight& spotLight,
                  const glm::vec3& pointLightPosition, _PointLight* pointLight,
                  const MaterialConfig& materialConfig);
  void copyDepthBuffer(uint32_t windowWidth, uint32_t windowHeight);

  //  private:
  // deferred shading data
  Shader geometryShader_;
  Shader lightShader_;
  unsigned int gBuffer_{0};
  unsigned int gPosition_{0}, gNormal_{0}, gAlbedoSpec_{0};
  unsigned int rboDepth_{0};
  unsigned int quadVAO_{0};
  // shadowmap data;
  Shader shadowMapShader_;
  unsigned int shadowMapBuffer_{0};
  unsigned int shadowMap_{0};

  void initRenderData();
  // tell OpenGL which color attachments we'll use (of this framebuffer) for
  // rendering
};