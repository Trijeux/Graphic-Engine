#ifndef GPR924_COMPGRAPH_CUBE_LIGHT_H
#define GPR924_COMPGRAPH_CUBE_LIGHT_H

#include <glm/vec3.hpp>

#include "engine/renderer.h"
#include "engine/system.h"
#include "engine/window.h"
#include "shader_learn_opengl.h"

namespace graphics {
class CubeLight final : public common::SystemInterface,
                        public common::DrawInterface {
public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void FixedUpdate() override;
  void Draw() override;

private:
  std::string path_texture = "data/textures/";
  // settings
  const unsigned int SCR_WIDTH = 800;
  const unsigned int SCR_HEIGHT = 600;

  // camera
  glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
  float lastX = common::GetWindowSize().x / 2.0f;
  float lastY = common::GetWindowSize().y / 2.0f;
  bool  firstMouse = true;

  // timing
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // lighting
  glm::vec3    lightPos = glm::vec3(1.2f, 2.0f, 2.0f);
  glm::mat4    model = glm::mat4(1.0f);
  unsigned int VBO = 0, VBOLight = 0, cubeVAO = 0, lightCubeVAO = 0, textureDiffuse = 0,
  textureSpecular = 0, textureNormal = 0, frameBuffer = 0, colorBuffer = 0, depthRBO = 0, quadVAO = 0, quadVBO = 0;
  Shader       Cube = Shader(), cube_light = Shader(), post_light = Shader(), g_buffer = Shader();

};
}
#endif //GPR924_COMPGRAPH_CUBE_LIGHT_H