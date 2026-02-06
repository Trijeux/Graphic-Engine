#include "engine/window.h"

#include <imgui.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "FinalProject/Final_Scene.h"

#include "4-cube_3d.h"

namespace Graphics {
void Final_Scene::CreatMultiModel(glm::vec3 pos) {
  ModelTransform MultiModel;
  MultiModel.position = glm::vec3(pos);
  MultiModel.scale = glm::vec3(1.f);
  MultiModel.is_rotating = false;
  MultiModelInstances.push_back(MultiModel);
}

void Final_Scene::Begin() {
  currentWindowSize = common::GetWindowSize();

  // Init Camera
  camera.Init({0, 5.0f, 30.0f});

  //Init FrameBuffer
  framebuffer.Init();

  //Init ShadowMap
  shadowMap.Init();
  pointShadowMap.Init();

  common::Shader sVert, sFrag;
  sVert.Load("data/shaders/FinalProject/shadow_depth.vert", GL_VERTEX_SHADER);
  sFrag.Load("data/shaders/FinalProject/shadow_depth.frag", GL_FRAGMENT_SHADER);
  shadowPipeline.Load(sVert, sFrag);

  // --- Init G-Buffer (Deferred) ---
  gBuffer.Init(currentWindowSize.x, currentWindowSize.y);
  ssao_.Init(currentWindowSize.x, currentWindowSize.y);

  // Setup Lights
  // A. Directional Light (The Sun)
  DirectionalLight sun{glm::vec3(-0.2f, -1.0f, -1.0f),
                       glm::vec3(1.f, 1.f, 1.f)};
  lightManager.SetDirectionalLight(sun);

  // B. FlashLight
  SpotLight flash{camera.Position, camera.Front, glm::vec3(1.f, 1.f, 1.f)};
  lightManager.SetFlashLight(flash);

  // --- Init Model Instance VBO ---
  for (int i = 0; i < modelMulti.size(); ++i) {
    MultiModelInstanceVBO.push_back(0);
    glGenBuffers(1, &MultiModelInstanceVBO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, MultiModelInstanceVBO[i]);
    glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(glm::mat4), nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  // --- Init Other Model Instance VBO ---
  for (int i = 0; i < modelSolo.size(); ++i) {
    SoloModelInstanceVBO.push_back(0);
    glGenBuffers(1, &SoloModelInstanceVBO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, SoloModelInstanceVBO[i]);
    glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(glm::mat4), nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  // --- Init Cube Instance VBO ---
  glGenBuffers(1, &cubeInstanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);
  glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(glm::mat4), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Load Models
  for (int i = 0; i < modelMulti.size(); ++i) {
    MultiModelACircle.emplace_back();
    MultiModelACircle[i].Load(modelMulti[i]);
    MultiModelACircle[i].SetupInstancing(MultiModelInstanceVBO[i]);
  }
  // Load other Model
  for (int i = 0; i < modelSolo.size(); ++i) {
    SoloModelCenter.emplace_back();
    SoloModelCenter[i].Load(modelSolo[i]);
    SoloModelCenter[i].SetupInstancing(SoloModelInstanceVBO[i]);
  }

  //Init Cube
  cubeMesh.Init();
  cubeMesh.SetupInstancing(cubeInstanceVBO);

  // Spawn model instances
  CreatMultiModel({15.f, 0.f, 0.f});
  CreatMultiModel({0.f, 0.f, 15.f});
  CreatMultiModel({0.f, 0.f, -15.f});
  CreatMultiModel({-15.f, 0.f, 0.f});

  CreatMultiModel({15.f, 0.f, 15.f});
  CreatMultiModel({-15.f, 0.f, 15.f});
  CreatMultiModel({15.f, 0.f, -15.f});
  CreatMultiModel({-15.f, 0.f, -15.f});

  // Spawn other model instances
  ModelTransform SoloModel;
  SoloModel.position = glm::vec3(0.0f, 0.f, 0.0f);
  SoloModel.scale = glm::vec3(1.f);
  SoloModel.is_rotating = false;
  SoloModelInstances.push_back(SoloModel);

  // Spawn cube instances
  CubeTransform cubeInst;
  cubeInst.position = glm::vec3(0.0f, -50.0f, 0.0f);
  cubeInst.scale = glm::vec3(100.0f, 100.0f, 100.0f);
  cubeInst.rotates = false;
  cubeInstances.push_back(cubeInst);

  // Init Light Gizmo
  lightGizmo.Init();

  // Init Skybox
  skybox.Init();
}

void Final_Scene::End() {
  MultiModelInstances.clear();
  cubeInstances.clear();
  lightManager.Clear();
  lightGizmo.Destroy();
  cubeMesh.Destroy();
  skybox.Destroy();
  gBuffer.Destroy();
  ssao_.Destroy();
  pointShadowMap.Destroy();
  glDeleteBuffers(1, &cubeInstanceVBO);
}

void Final_Scene::Update(const float dt) {
  auto newSize = common::GetWindowSize();

  if ((newSize.x != currentWindowSize.x || newSize.y != currentWindowSize.y) &&
      newSize.x > 0 && newSize.y > 0) {
    currentWindowSize = newSize;
    framebuffer.Resize(newSize.x, newSize.y);
    gBuffer.Resize(newSize.x, newSize.y);
    ssao_.Resize(newSize.x, newSize.y);
  }

  // Safety check to avoid division by zero if window is minimized
  float aspectRatio = (currentWindowSize.y > 0)
                        ? static_cast<float>(currentWindowSize.x) / static_cast<
                            float>(currentWindowSize.y)
                        : 1.0f;
  proj = glm::perspective(glm::radians(camera.Fov), aspectRatio, zNear, zFar);

  // --- Update Instances ---
  for (auto& inst : MultiModelInstances) inst.Update(dt);
  for (auto& inst : SoloModelInstances) inst.Update(dt);
  for (auto& inst : cubeInstances) inst.Update(dt);

  view = glm::lookAt(camera.Position, camera.Position + camera.Front,
                     camera.Up);

  //Update Camera Movement && Frustrum
  glm::mat4 viewProj = proj * view;
  camera.Update(dt, viewProj);

  // Update Flashlight to follow camera
  if (lightManager.hasFlashLight) {
    lightManager.UpdateFlashLight(camera.Position, camera.Front);
  }
  glm::vec3 pos_modifior = glm::vec3(0.0f, 5.0f, 0.0f);
  float     rad_modifior = 0.0f;
  if (soloModelActive && modelSelected == ModelSelected::Behemoth) {
    pos_modifior = glm::vec3(0.0f, 12.5f, 0.0f);
  }
  if (soloModelActive && modelSelected == ModelSelected::Behemoth) {
    rad_modifior = 10.f;
  }
  if (soloModelActive && modelSelected == ModelSelected::Hollow_Knight) {
    rad_modifior = 4.f;
  }
  if (soloModelActive && modelSelected == ModelSelected::tonberry) {
    rad_modifior = 2.f;
  }

  if (disco && soloModelActive) {
    lightManager.RotatesPointLightsAround(
        SoloModelInstances[0].position + pos_modifior, rad_modifior);
  }
}

void Final_Scene::FixedUpdate() {
}

void Final_Scene::Draw() {
  // =================================================================================
  // PASS 0: SHADOW PASS (Render Depth from Sun's POV)
  // =================================================================================
  shadowMap.BindForWriting();

  // 1. STATE FIX: Explicitly enable Depth Test (Critical!)
  glEnable(GL_DEPTH_TEST);

  // 2. ACNE FIX: Cull FRONT faces.
  // This renders back-faces to the shadow map, preventing self-shadowing artifacts.
  glCullFace(GL_FRONT);

  // 3. FRUSTUM FIX: Increase bounds to match your scene scale
  // Your floor is huge, so we need a larger area and deeper view.
  float orthoSize = 100.0f; // Increased from 30 to 100 to cover the floor
  float near_plane = 1.0f;
  float far_plane = 250.0f;
  // Increased from 100 to 250 to catch the bottom of the floor

  glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize,
                                         orthoSize, near_plane, far_plane);

  // 4. VIEW FIX: Ensure light is high enough
  glm::vec3 lightPos = glm::vec3(0, 0, 0) - (
                         lightManager.dirLight.direction * 100.0f);
  // Move light back 100 units
  glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f),
                                    glm::vec3(0.0, 1.0, 0.0));

  lightSpaceMatrix = lightProjection * lightView;

  shadowPipeline.Bind();
  shadowPipeline.SetMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));

  // Draw Scene for Shadows
  if (multiModelActive && modelSelected != ModelSelected::Fang) {
    glBindBuffer(
        GL_ARRAY_BUFFER,
        MultiModelInstanceVBO[static_cast<int>(sceneSelected)]);
    MultiModelACircle[static_cast<int>(sceneSelected)].Draw(shadowPipeline,
      MultiModelInstances.size());
  }
  if (soloModelActive) {
    glBindBuffer(
        GL_ARRAY_BUFFER, SoloModelInstanceVBO[static_cast<int>(modelSelected)]);
    SoloModelCenter[static_cast<int>(modelSelected)].
        Draw(shadowPipeline, SoloModelInstances.size());
  }
  if (modelSelected != ModelSelected::Fang) {
    glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);
    cubeMesh.DrawInstanced(shadowPipeline, cubeInstances.size());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  // 1. Setup Point Light Shadows
  // Loop through lights up to the limit (e.g., 4)
  int shadowCasters = std::min((int)lightManager.pointLights.size(),
                               pointShadowMap.maxShadowLights);

  if (shadowCasters > 0) {
    float     aspect = 1.0f;
    float     near = 1.0f;
    float     far = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near,
                                            far);

    pointShadowMap.pipeline.Bind();
    pointShadowMap.pipeline.SetFloat("far_plane", far);
    glEnable(GL_DEPTH_TEST);
    // Loop through each shadow-casting light
    for (int k = 0; k < shadowCasters; ++k) {
      PointLight& light = lightManager.pointLights[k];

      pointShadowMap.pipeline.SetVec3("lightPos", light.position.x,
                                      light.position.y, light.position.z);

      std::vector<glm::mat4> shadowTransforms;
      shadowTransforms.push_back(
          shadowProj * glm::lookAt(light.position,
                                   light.position + glm::vec3(1, 0, 0),
                                   glm::vec3(0, -1, 0)));
      shadowTransforms.push_back(
          shadowProj * glm::lookAt(light.position,
                                   light.position + glm::vec3(-1, 0, 0),
                                   glm::vec3(0, -1, 0)));
      shadowTransforms.push_back(
          shadowProj * glm::lookAt(light.position,
                                   light.position + glm::vec3(0, 1, 0),
                                   glm::vec3(0, 0, 1)));
      shadowTransforms.push_back(
          shadowProj * glm::lookAt(light.position,
                                   light.position + glm::vec3(0, -1, 0),
                                   glm::vec3(0, 0, -1)));
      shadowTransforms.push_back(
          shadowProj * glm::lookAt(light.position,
                                   light.position + glm::vec3(0, 0, 1),
                                   glm::vec3(0, -1, 0)));
      shadowTransforms.push_back(
          shadowProj * glm::lookAt(light.position,
                                   light.position + glm::vec3(0, 0, -1),
                                   glm::vec3(0, -1, 0)));
      // Render 6 faces for THIS light (k)
      for (int i = 0; i < 6; ++i) {
        // Bind light k, face i
        pointShadowMap.BindForWriting(k, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

        pointShadowMap.pipeline.SetMat4("shadowMatrices",
                                        glm::value_ptr(shadowTransforms[i]));

        // Draw Scene
        if (soloModelActive) {
          glBindBuffer(
              GL_ARRAY_BUFFER,
              SoloModelInstanceVBO[static_cast<int>(modelSelected)]);
          SoloModelCenter[static_cast<int>(modelSelected)].Draw(
              pointShadowMap.pipeline,
              static_cast<int>(SoloModelInstances.size()));
        }
        glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);
        cubeMesh.DrawInstanced(pointShadowMap.pipeline,
                               static_cast<int>(cubeInstances.size()));
      }
    }
  }

  // 5. RESTORE STATE: Switch culling back to normal
  glCullFace(GL_BACK);

  // Reset Viewport for the main screen (Window Size)
  glViewport(0, 0, currentWindowSize.x, currentWindowSize.y);

  // =================================================================================
  // PASS 1: GEOMETRY PASS (Render to G-Buffer)
  // =================================================================================
  gBuffer.Bind();

  glEnable(GL_DEPTH_TEST); // Ensure it's enabled for G-Buffer too

  glDisable(GL_CULL_FACE);

  gBuffer.geometryPipeline.Bind();
  gBuffer.geometryPipeline.SetMat4("view", glm::value_ptr(view));
  gBuffer.geometryPipeline.SetMat4("projection", glm::value_ptr(proj));
  gBuffer.geometryPipeline.SetInt("useNormalMap", enableNormalMapping);

  // --- Models Draw Loop ---
  if (multiModelActive && modelSelected != ModelSelected::Fang) {
    visibleMultiModels.clear();
    visibleMultiModels.reserve(MultiModelInstances.size());
    for (auto& inst : MultiModelInstances) {
      glm::vec3 center = inst.position + (
                           MultiModelACircle[static_cast<int>(sceneSelected)].
                           boundingCenter * inst.
                           scale);
      float radius = MultiModelACircle[static_cast<int>(sceneSelected)].
                     boundingRadius * std::max(
                         inst.scale.x, std::max(inst.scale.y, inst.scale.z));
      if (camera.IsOnFrustum(center, radius)) {
        visibleMultiModels.push_back(inst.model);
      }
    }

    if (!visibleMultiModels.empty()) {
      glBindBuffer(
          GL_ARRAY_BUFFER,
          MultiModelInstanceVBO[static_cast<int>(sceneSelected)]);
      glBufferSubData(
          GL_ARRAY_BUFFER, 0, visibleMultiModels.size() * sizeof(glm::mat4),
          visibleMultiModels.data());
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      MultiModelACircle[static_cast<int>(sceneSelected)].Draw(
          gBuffer.geometryPipeline,
          visibleMultiModels.size());
    }
  }
  // --- Other Models Draw Loop ---
  if (soloModelActive) {
    visibleSoloModels.clear();
    visibleSoloModels.reserve(SoloModelInstances.size());
    for (auto& inst : SoloModelInstances) {
      glm::vec3 center = inst.position + (
                           SoloModelCenter[static_cast<int>(modelSelected)].
                           boundingCenter * inst.
                           scale);
      float radius = SoloModelCenter[static_cast<int>(modelSelected)].
                     boundingRadius * std::max(
                         inst.scale.x, std::max(inst.scale.y, inst.scale.z));

      if (camera.IsOnFrustum(center, radius)) {
        visibleSoloModels.push_back(inst.model);
      }
    }

    if (!visibleSoloModels.empty()) {
      glBindBuffer(
          GL_ARRAY_BUFFER,
          SoloModelInstanceVBO[static_cast<int>(modelSelected)]);
      glBufferSubData(
          GL_ARRAY_BUFFER, 0, visibleSoloModels.size() * sizeof(glm::mat4),
          visibleSoloModels.data());
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // Ensure Mario uses normals if available
      SoloModelCenter[static_cast<int>(modelSelected)].Draw(
          gBuffer.geometryPipeline,
          visibleSoloModels.size());
    }
  }
  if (modelSelected != ModelSelected::Fang) {
    // --- Cubes Draw Loop ---
    visibleCubes.clear();
    visibleCubes.reserve(cubeInstances.size());
    for (auto& inst : cubeInstances) {
      auto  center = glm::vec3(inst.model[3]);
      float radius = cubeMesh.boundingRadius * std::max(
                         inst.scale.x, std::max(inst.scale.y, inst.scale.z));
      if (camera.IsOnFrustum(center, radius)) {
        visibleCubes.push_back(inst.model);
      }
    }

    if (!visibleCubes.empty()) {
      glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);
      glBufferSubData(
          GL_ARRAY_BUFFER, 0, visibleCubes.size() * sizeof(glm::mat4),
          visibleCubes.data());
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      cubeMesh.DrawInstanced(gBuffer.geometryPipeline, visibleCubes.size());
    }
  }
  gBuffer.Unbind();

  // =================================================================================
  // PASS 1.5: SSAO PASS
  //
  ssao_.Render(gBuffer.gPosition, gBuffer.gNormal, proj, view,
               framebuffer.quadVAO, ssaoRadius, ssaoBias);

  // =================================================================================
  // PASS 2: LIGHTING PASS (Render to HDR Framebuffer)
  // =================================================================================
  framebuffer.Bind();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gBuffer.lightingPipeline.Bind();

  // Bind G-Buffer Textures (Units 0, 1, 2)
  gBuffer.BindTextures();
  gBuffer.lightingPipeline.SetInt("gPosition", 0);
  gBuffer.lightingPipeline.SetInt("gNormal", 1);
  gBuffer.lightingPipeline.SetInt("gAlbedoSpec", 2);
  gBuffer.lightingPipeline.SetInt("debugMode", gBufferDebugMode);
  gBuffer.lightingPipeline.SetInt("useSSAO", enableSSAO);

  gBuffer.lightingPipeline.SetVec3("viewPos", camera.Position.x,
                                   camera.Position.y, camera.Position.z);
  gBuffer.lightingPipeline.SetFloat("specularPow", specularPow);

  // Bind Shadow Map (Unit 3)
  shadowMap.BindForReading(GL_TEXTURE3);
  gBuffer.lightingPipeline.SetInt("shadowMap", 3);
  gBuffer.lightingPipeline.SetMat4("lightSpaceMatrix",
                                   glm::value_ptr(lightSpaceMatrix));

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, ssao_.ssaoBlurBuffer);
  gBuffer.lightingPipeline.SetInt("ssao", 4);

  // Unit 5 for Point Shadow Map
  pointShadowMap.BindForReading(GL_TEXTURE5);
  for (int i = 0; i < pointShadowMap.maxShadowLights; i++) {
    std::string uniformName = "pointShadowMaps[" + std::to_string(i) + "]";
    gBuffer.lightingPipeline.SetInt(uniformName, 5 + i);
  }
  gBuffer.lightingPipeline.SetFloat("pointFarPlane", 25.0f);
  // Match the draw step

  // Apply Lights
  lightManager.ApplyLighting(gBuffer.lightingPipeline);

  // Draw Screen Quad
  glBindVertexArray(framebuffer.quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // =================================================================================
  // PASS 3: FORWARD RENDERING
  // =================================================================================
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.gBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.fbo);
  glBlitFramebuffer(0, 0, currentWindowSize.x, currentWindowSize.y,
                    0, 0, currentWindowSize.x, currentWindowSize.y,
                    GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  framebuffer.Bind();

  // Draw Gizmos
  for (const auto& light : lightManager.pointLights) {
    lightGizmo.Draw(view, proj, light.position, light.color, lightGizmoScale);
  }
  for (const auto& light : lightManager.spotLights) {
    lightGizmo.Draw(view, proj, light.position, light.color, lightGizmoScale);
  }
  if (lightManager.hasDirLight) {
    skybox.Draw(view, proj);
  }

  // =================================================================================
  // PASS 4: POST PROCESSING
  // =================================================================================
  framebuffer.Unbind();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  framebuffer.Draw();
}

void Final_Scene::OnGui() {
  //ImGui::GetIO().FontGlobalScale = 1.5f;

  ImGui::Begin("Scene Control");

  if (ImGui::BeginTabBar("Scene Controler")) {
    if (ImGui::BeginTabItem("Camera")) {
      if (ImGui::BeginTabBar("Camera Controler")) {
        if (ImGui::BeginTabItem("Control")) {
          ImGui::SeparatorText("");
          ImGui::Text("W/Z = Forward");
          ImGui::Text("S = Backward");
          ImGui::Text("A/Q = Left");
          ImGui::Text("D = Right");
          ImGui::SeparatorText("");
          ImGui::Text("Space = Up");
          ImGui::Text("Left Ctrl = Down");
          ImGui::Text("Left Shift = Speed Up");
          ImGui::SeparatorText("");
          ImGui::Text("Right Click Mouse = Rotate Cam");
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Value")) {
          ImGui::DragFloat3("Position", &camera.Position.x, 0.1f);
          ImGui::DragFloat("Speed", &camera.moveSpeed, 0.1f, 1.f, 100.f);
          ImGui::DragFloat("SpeedMult", &camera.moveSpeedMult, 1.f, 1.f, 10.f);
          ImGui::DragFloat("FOV", &camera.Fov, 0.1f, 10.f, 120.0f);
          ImGui::DragFloat("zNear", &zNear, 0.1f, 0.5f, 10.f);
          ImGui::DragFloat("zFar", &zFar, 0.1f, 10.f, 1000.f);
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }
      ImGui::EndTabItem();
    }

  if (ImGui::BeginTabItem("Models")) {
    const char* modelsSolo[] = {
        "Behemoth", "Fang", "Hollow Knight", "tonberry", "Vivi", "None"
    };
    if (ImGui::BeginCombo("Model Center",
                          modelsSolo[static_cast<int>(modelSelected)])) {
      for (int i = 0; i < modelSolo.size() + 1; i++) {
        bool isSelected = (static_cast<int>(modelSelected) == i);
        if (ImGui::Selectable(modelsSolo[i], isSelected)) {
          soloModelActive = true;
          modelSelected = static_cast<ModelSelected>(i);
        }
        if (static_cast<int>(modelSelected) == 5) {
          soloModelActive = false;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    const char* modelsMulti[] = {
        "Cloud Sowrd", "Gunblade", "Hollow soul", "None"
    };
    if (modelSelected != ModelSelected::Fang) {
      if (ImGui::BeginCombo("Model A Round",
                            modelsMulti[static_cast<int>(sceneSelected)])) {
        for (int i = 0; i < modelMulti.size() + 1; i++) {
          multiModelActive = true;
          bool isSelected = (static_cast<int>(sceneSelected) == i);
          if (ImGui::Selectable(modelsMulti[i], isSelected)) {
            sceneSelected = static_cast<SceneSelected>(i);
          }
          if (static_cast<int>(sceneSelected) == 3) {
            multiModelActive = false;
          }
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }
    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("Post Processing")) {
    ImGui::Checkbox("Enable Bloom", &framebuffer.useBloom);
    ImGui::DragFloat("Exposure", &framebuffer.exposure, 0.1f, 0.1f, 10.0f);

    const char* modes[] = {
        "None", "Inverse", "Grayscale", "Sharpen", "Blur", "Edge Detect",
        "DEBUG BLOOM LAYER"
    };
    if (ImGui::BeginCombo("Effect", modes[framebuffer.postProcessMode])) {
      for (int i = 0; i < 7; i++) {
        bool isSelected = (framebuffer.postProcessMode == i);
        if (ImGui::Selectable(modes[i], isSelected)) {
          framebuffer.postProcessMode = i;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::EndTabItem();
  }


  if (ImGui::BeginTabItem("Deferred Debug")) {
    ImGui::Checkbox("Enable SSAO", &enableSSAO);
    ImGui::SliderFloat("ssao Radius", &ssaoRadius, 0.1f, 2.5f);
    ImGui::SliderFloat("ssao Bias", &ssaoBias, 0.001f, 1.f);
    const char* debugModes[] = {"Combined", "Position", "Normal", "Albedo",
                                "Specular"};
    if (ImGui::BeginCombo("G-Buffer View", debugModes[gBufferDebugMode])) {
      for (int i = 0; i < 5; i++) {
        bool isSelected = (gBufferDebugMode == i);
        if (ImGui::Selectable(debugModes[i], isSelected)) {
          gBufferDebugMode = i;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("Lights")) {
    if (ImGui::BeginTabBar("Light Controler")){
      if (ImGui::BeginTabItem("General Light")) {
        ImGui::SliderFloat("LightGizmoScale", &lightGizmoScale, 0.1f, 1.f);
        ImGui::Checkbox("Use Normal Mapping", &enableNormalMapping);
        ImGui::EndTabItem();
      }
    if (ImGui::BeginTabItem("Sun Light")) {
      ImGui::Checkbox("Enable Sun", &lightManager.hasDirLight);
      ImGui::SliderFloat3("Direction", &lightManager.dirLight.direction.x,
                          -1.0f, 1.0f);
      ImGui::ColorEdit3("Sun Color", &lightManager.dirLight.color.x);
      ImGui::SliderFloat("Ambient", &lightManager.dirLight.ambientStrength,
                         0.0f, 1.0f);
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Flash Light")) {
      ImGui::Checkbox("Enable Flashlight", &lightManager.hasFlashLight);
      ImGui::ColorEdit3("Flashlight Color", &lightManager.flashLight.color.x);
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Point Lights")) {
      if (ImGui::Button("Add Point Light")) {
        PointLight p{glm::vec3(0, 0, -15), glm::vec3(1.f, 1.f, 1.f)};
        lightManager.AddPointLight(p);
      }
      if (ImGui::Button("Disco Mod")) {
        disco = !disco;
        lightManager.pointLights.clear();
        if (disco) {
          PointLight p{glm::vec3(0, 0, 0), glm::vec3(1.f, 1.f, 1.f)};
          lightManager.AddPointLight(p);
        }
      }
      for (int i = 0; i < lightManager.pointLights.size(); i++) {
        std::string label = "Point Light " + std::to_string(i);
        if (ImGui::TreeNode(label.c_str())) {
          ImGui::DragFloat3("Pos", &lightManager.pointLights[i].position.x,
                            0.1f);
          ImGui::ColorEdit3("Color", &lightManager.pointLights[i].color.x);
          if (ImGui::Button("Remove Point Light")) {
            lightManager.RemovePointLight(i);
            ImGui::TreePop();
            break;
          }
          ImGui::TreePop();
        }
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Spot Lights")) {
      if (ImGui::Button("Add Spot Light")) {
        SpotLight spot{glm::vec3(0, 0, -15), glm::vec3(0, -1, 0),
                       glm::vec3(1.f, 1.f, 1.f)};
        lightManager.AddSpotLight(spot);
      }

      for (int i = 0; i < lightManager.spotLights.size(); i++) {
        std::string label = "Spot Light " + std::to_string(i);
        if (ImGui::TreeNode(label.c_str())) {
          ImGui::DragFloat3("Pos", &lightManager.spotLights[i].position.x,
                            0.1f);
          ImGui::DragFloat3("Dir", &lightManager.spotLights[i].direction.x,
                            0.1f);
          ImGui::ColorEdit3("Color", &lightManager.spotLights[i].color.x);
          if (ImGui::Button("Remove Spot Light")) {
            lightManager.RemoveSpotLight(i);
            ImGui::TreePop();
            break;
          }
          ImGui::TreePop();
        }
      }
      ImGui::EndTabItem();
    }
      ImGui::EndTabBar();
    }
    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("Cubes")) {
    if (ImGui::Button("Add Cube")) {
      CubeTransform cube;
      cube.position = glm::vec3(2.0f, -3.0f, 0.0f);
      cube.rotates = false;
      cubeInstances.push_back(cube);
    }
    int idx;
    idx = modelSelected != ModelSelected::Fang ? 0 : 1;
    for (int i = idx; i < cubeInstances.size(); i++) {
      std::string label = "Cube " + std::to_string(i);
      if (ImGui::TreeNode(label.c_str())) {
        ImGui::DragFloat3("Position", &cubeInstances[i].position.x, 0.1f);
        ImGui::DragFloat3("Scale", &cubeInstances[i].scale.x, 0.1f);
        ImGui::Checkbox("Rotate", &cubeInstances[i].rotates);
        if (cubeInstances.size() > 1 && i > 0) {
          if (ImGui::Button("Remove Cube")) {
            cubeInstances.erase(cubeInstances.begin() + i);
            ImGui::TreePop();
            break;
          }
        }
        ImGui::TreePop();
      }
    }
    ImGui::EndTabItem();
  }
    ImGui::EndTabBar();
  }
  ImGui::End();
  //FPS GUI
  // 1. Setup Position (Top-Right Corner)
  const float PAD = 10.0f;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu bars
  ImVec2 work_size = viewport->WorkSize;

  ImVec2 window_pos, window_pos_pivot;
  window_pos.x = work_pos.x + work_size.x - PAD;
  window_pos.y = work_pos.y + PAD;
  window_pos_pivot.x = 1.0f; // Pivot at right
  window_pos_pivot.y = 0.0f; // Pivot at top

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  // 2. Setup Style (Transparent background, No Title bar, No Resize)
  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings |
                                  ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav |
                                  ImGuiWindowFlags_NoInputs;
  // Allow clicking through it

  // 3. Draw
  if (ImGui::Begin("FPS Overlay", nullptr, window_flags)) {
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
  }
  ImGui::End();
}
}