#ifndef GPR924_COMPGRAPH_CUBE_LIGHT_H
#define GPR924_COMPGRAPH_CUBE_LIGHT_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "engine/renderer.h"
#include "engine/system.h"
#include "engine/window.h"
#include "shader_learn_opengl.h"

namespace graphics {
class CubeLightRendering final : public common::SystemInterface,
                                 public common::DrawInterface {
public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void FixedUpdate() override; // Nécessaire pour corriger LNK2001
  void Draw() override;

private:
  std::string path_texture = "data/textures/";

  // Shader et VAO
  Shader Cube;
  Shader light;
  unsigned int cubeVAO, VBO;

  // G-Buffer
  unsigned int g_buffer;
  unsigned int g_position, g_normal, albedo;

  // Textures du cube
  unsigned int textureDiffuse, textureSpecular, textureNormal;

  // Camera & Lumière
  glm::vec3 camPos = glm::vec3(0.0f, 0.5f, 5.0f);
  glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 lightPos = glm::vec3(2.0f, 1.0f, 2.0f);
  glm::mat4 model = glm::mat4(1.0f);
};
}
#endif