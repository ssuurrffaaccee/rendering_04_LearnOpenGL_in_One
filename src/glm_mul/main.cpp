#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
std::string toString(const glm::mat4& m4) {
  std::stringstream ss;
  ss << "mat4[\n";
  ss << m4[0][0] << "," << m4[1][0] << "," << m4[2][0] << "," << m4[3][0]
     << "\n";
  ss << m4[0][1] << "," << m4[1][1] << "," << m4[2][1] << "," << m4[3][1]
     << "\n";
  ss << m4[0][2] << "," << m4[1][2] << "," << m4[2][2] << "," << m4[3][2]
     << "\n";
  ss << m4[0][3] << "," << m4[1][3] << "," << m4[2][3] << "," << m4[3][3]
     << "]\n";
  return ss.str();
}
std::string toString(const glm::mat3& m3) {
  std::stringstream ss;
  ss << "mat4[\n";
  ss << m3[0][0] << "," << m3[1][0] << "," << m3[2][0] << "\n";
  ss << m3[0][1] << "," << m3[1][1] << "," << m3[2][1] << "\n";
  ss << m3[0][2] << "," << m3[1][2] << "," << m3[2][2] << "\n";
  return ss.str();
}
std::string toString(const glm::mat2& m2) {
  std::stringstream ss;
  ss << "mat2[\n";
  ss << m2[0][0] << "," << m2[1][0] << "\n";
  ss << m2[0][1] << "," << m2[1][1] << "\n";
  return ss.str();
}
std::string toString(const glm::vec2& v2) {
  std::stringstream ss;
  ss << "vec2(" << v2[0] << "," << v2[1] << ")\n";
  return ss.str();
}
std::string toString(const glm::vec3& v3) {
  std::stringstream ss;
  ss << "vec2(" << v3[0] << "," << v3[1] << "," << v3[2] << ")\n";
  return ss.str();
}
inline glm::vec3 eulerAngleToDirection(const glm::vec3& baseDirection,
                                       const glm::vec3& eularAngle) {
  auto model = glm::mat4(1.0f);
  model = glm::rotate(model, eularAngle[2], glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::rotate(model, eularAngle[1], glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, eularAngle[0], glm::vec3(1.0f, 0.0f, 0.0f));
  return glm::vec4{baseDirection, 1.0f} * model;
};
int main() {
  glm::mat3 m{1.0f, 2.0f, 3.0f, 1.0f, 2.0f, 3.0f, 1.0f, 2.0f, 3.0f};
  glm::vec3 v{1.0f, 1.0f, 1.0f};
  std::cout << toString(m * v) << "\n";
  std::cout << toString(v * m) << "\n";
  auto baseDirection = glm::vec3{1.0f, 0.0f, 0.0f};
  std::cout << toString(
      eulerAngleToDirection(baseDirection, glm::vec3{0.0f, 0.0f, 3.1415f/2.0f}));
  return 0;
}