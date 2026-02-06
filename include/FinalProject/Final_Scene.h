#ifndef GPR924_COMPGRAPH_SCENE_H
#define GPR924_COMPGRAPH_SCENE_H

#include <vector>
#include <glm/glm.hpp>

#include "camera.h"
#include "frame_buffer.h"
#include "point_shadow_map.h"
#include "skybox.h"
#include "ssao.h"
#include "FinalProject/cube_transform.h"
#include "FinalProject/g_buffer.h"
#include "FinalProject/instancing_cube_mesh.h"
#include "FinalProject/instancing_model.h"
#include "FinalProject/light_gizmo.h"
#include "FinalProject/light_manager.h"
#include "FinalProject/model_transform.h"
#include "FinalProject/shadow_map.h"
#include "engine/gui.h"
#include "engine/renderer.h"
#include "engine/system.h"

namespace Graphics {
enum class LightType {
  point,
  directional,
  spot,
  flash,
  length
};

class Final_Scene final : public common::SystemInterface,
                          public common::OnGuiInterface,
                          public common::DrawInterface {
  const std::array<std::string, 5> modelSolo = {
      "data/models/behemoth-king-final-fantasy-x/behemoth king.obj",
      "data/models/eruyt-project-2017/Fang.obj",
      "data/models/hollow_knight/hollow_knight.obj",
      "data/models/tonberry-ffx/tonberry.obj",
      "data/models/vivi-ornitier-retrogasm-art-competition/Vivi_Final.obj",
  };

  const std::array<std::string, 3> modelMulti = {
      "data/models/final_fantasy_7_buster_sword/Sword_FF.obj",
      "data/models/gunblade-from-ffviii/Gun_Blade.obj",
      "data/models/hollow-knighthigh-poly/hollow knight(high poly).obj",
  };

  const std::array<glm::vec3, 8> posSoul = {
      glm::vec3(10.f, 0.f, 0.f),
      glm::vec3(0.f, 0.f, 10.f),
      glm::vec3(0.f, 0.f, -10.f),
      glm::vec3(-10.f, 0.f, 0.f),
      glm::vec3(10.f, 0.f, 10.f),
      glm::vec3(-10.f, 0.f, 10.f),
      glm::vec3(10.f, 0.f, -10.f),
      glm::vec3(-10.f, 0.f, -10.f)
  };

  // --- Resources ---
  std::vector<InstancingModel> SoloModelCenter;
  std::vector<InstancingModel> MultiModelACircle;


  InstancingCubeMesh cubeMesh;
  Skybox             skybox;
  GBuffer            gBuffer;

  // --- Instancing Data ---
  std::vector<GLuint> SoloModelInstanceVBO;
  std::vector<GLuint> MultiModelInstanceVBO;

  std::vector<glm::mat4> visibleMultiModels;
  std::vector<glm::mat4> visibleCubes;
  std::vector<glm::mat4> visibleSoloModels;
  GLuint                 cubeInstanceVBO = 0;


  // --- Lighting ---
  LightManager lightManager = {};
  LightGizmo   lightGizmo;

  // --- Scene Objects ---
  std::vector<ModelTransform> MultiModelInstances;
  std::vector<CubeTransform>  cubeInstances;
  std::vector<ModelTransform> SoloModelInstances;


  // --- Camera ---
  glm::mat4   proj = {};
  glm::mat4   view = {};
  Camera      camera = {};
  core::Vec2I currentWindowSize = {0, 0};

  // --- Post Processing ---
  Framebuffer framebuffer;

  // -- Shadows ---
  ShadowMap        shadowMap;
  PointShadowMap   pointShadowMap;
  common::Pipeline shadowPipeline;
  glm::mat4        lightSpaceMatrix = {};
  SSAO             ssao_;

  // GUI variables
  float specularPow = 32.0f;
  float lightGizmoScale = 1.0f;
  float zNear = 1.f;
  float zFar = 100.0f;
  bool  enableNormalMapping = true;
  bool  enableSSAO = true;
  int   gBufferDebugMode = 0;
  float ssaoRadius = 0.5f;
  float ssaoBias = 0.025f;
  enum class ModelSelected{
    Behemoth,
    Fang,
    Hollow_Knight,
    tonberry,
    Vivi,
    None
  };
  enum class SceneSelected {
    Cloud_Sowrd,
    Gunblade,
    Hollow_soul,
    None
  };
  ModelSelected modelSelected = ModelSelected::Behemoth;
  SceneSelected sceneSelected = SceneSelected::Cloud_Sowrd;
  bool soloModelActive = true;
  bool multiModelActive = true;
  bool disco = false;

public:
  void CreatMultiModel(glm::vec3 pos);
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void FixedUpdate() override;
  void Draw() override;
  void OnGui() override;
};
}

#endif //GPR924_COMPGRAPH_SCENE_H