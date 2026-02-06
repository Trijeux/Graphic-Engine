#ifndef MODEL_SAMPLE_H
#define MODEL_SAMPLE_H
#include <array>

#include "6-model_instance.h" // New header
#include "6-model.h"
#include "engine/gui.h"
#include "engine/renderer.h"
#include "engine/system.h"

#include <vector>
#include <glm/glm.hpp>

#include "FinalProject/skybox.h"

namespace graphics {
class ModelSample final : public common::SystemInterface,
                          public common::OnGuiInterface,
                          public common::DrawInterface {
  struct Plane {
    glm::vec3 normal;
    float     distance;

    // Normalize the plane equation
    void Normalize() {
      float length = glm::length(normal);
      normal /= length;
      distance /= length;
    }
  };

  std::array<Plane, 6> planes = {};
  Model                nanoSuitModel;
  common::Pipeline     pipeline;
  common::Pipeline     screen_pipeline;
  Graphics::Skybox     skybox;
  unsigned int         frameBuffer = 0;
  unsigned int         colorBuffer = 0;
  unsigned int         depthRBO = 0;
  unsigned int         quadVAO = 0;
  unsigned int         quadVBO = 0;

  unsigned int instanceVBO = 0;
  std::vector<glm::mat4> visibleModels;

  int postProcess = 0;

  std::vector<ModelInstance> instances;

  // Camera
  glm::mat4 proj = {};
  glm::mat4 view = {};
  glm::vec3 camPos{0, 0, 5};
  glm::vec3 camFront{0, 0, -1};
  glm::vec3 camUp{0, 1, 0};
  float     fov = 45.f;
  float     yaw = -90.0f;
  float     pitch = 0.0f;
  float     moveSpeed = 5.0f;
  float     mouseSensitivity = 0.1f;

  // Lighting
  glm::vec3 lightColor{1, 1, 1};
  float     ambientStrength = 0.2f;
  float     specularPow = 32.f;
  glm::vec3 lightPos{0, 0, 0};
  bool      enableAttenuation = false;
  glm::vec3 lightDir{0, -1, 0};
  float     spotCutOff = 12.5f;
  float     spotOuterCutOff = 17.5f;

  bool IsOnFrustum(const glm::vec3& center, float radius) const;

public:
  void OnGui() override;
  void Draw() override;
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void FixedUpdate() override;
};
}

#endif //MODEL_SAMPLE_H