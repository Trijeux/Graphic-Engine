#include "6-model_sample.h"
#include <imgui.h>
#include <iostream>
#include "engine/window.h"
#include <glm/gtc/type_ptr.hpp>

namespace graphics {
void ModelSample::Begin() {
  // 1. Initialize Shaders
  common::Shader vertexShader;
  common::Shader fragmentShader;
  common::Shader vertexFrameBuffer;
  common::Shader fragmentFrameBuffer;

  vertexShader.Load("data/shaders/6-model.vert", GL_VERTEX_SHADER);
  fragmentShader.Load("data/shaders/6-model.frag", GL_FRAGMENT_SHADER);
  vertexFrameBuffer.Load("data/shaders/6-framebuffer_screen.vert", GL_VERTEX_SHADER);
  fragmentFrameBuffer.Load("data/shaders/6-framebuffer_screen.frag", GL_FRAGMENT_SHADER);
  pipeline.Load(vertexShader, fragmentShader);
  screen_pipeline.Load(vertexFrameBuffer, fragmentFrameBuffer);

  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, 10000 + sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // 2. Load the Model
  nanoSuitModel.Load("data/models/nanosuit2/nanosuit.obj");
  nanoSuitModel.SetupInstancing(instanceVBO);

  // 3. Create Model Instances
  for (int i = 0; i < 2; ++i) {
    ModelInstance inst;
    inst.position = glm::vec3(0 + i * 5, -4.f, -5.f);
    inst.scale = glm::vec3(0.5f);
    inst.rotates = true;
    inst.rotationAxis = glm::vec3(0, 1, 0);
    instances.push_back(inst);
  }

  skybox.Init();

  screen_pipeline.Bind();
  screen_pipeline.SetInt("screenTexture", 0);

  //FramBuffer

  float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions  // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

  // texture couleur (FRAMEBUFFER → PAS DE MIPMAP)
  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);

  // ⬇⬇⬇ IMPORTANT : désactiver les mipmaps ⬇⬇⬇
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               common::GetWindowSize().x, common::GetWindowSize().y, 0,
               GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, colorBuffer, 0);

  // depth
  glGenRenderbuffers(1, &depthRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, common::GetWindowSize().x, common::GetWindowSize().y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, depthRBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Framebuffer incomplete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModelSample::End() {
  instances.clear();
  skybox.Destroy();
  glDeleteBuffers(1, &instanceVBO);
}

void ModelSample::Update(const float dt) {
  // Update all instances
  for (auto &inst : instances)
    inst.Update(dt);

  // --- Camera Movement ---
  if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseDelta.x != 0 || io.MouseDelta.y != 0) {
      float offsetX = io.MouseDelta.x * mouseSensitivity;
      float offsetY = io.MouseDelta.y * mouseSensitivity;

      yaw   += offsetX;
      pitch -= offsetY;

      if (pitch > 89.0f) pitch = 89.0f;
      if (pitch < -89.0f) pitch = -89.0f;

      glm::vec3 front;
      front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      front.y = sin(glm::radians(pitch));
      front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      camFront = glm::normalize(front);
    }

    auto viewProj = proj * view;

    // Left Plane
    planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
    planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
    planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
    planes[0].distance = viewProj[3][3] + viewProj[3][0];

    // Right Plane
    planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
    planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
    planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
    planes[1].distance = viewProj[3][3] - viewProj[3][0];

    // Bottom Plane
    planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
    planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
    planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
    planes[2].distance = viewProj[3][3] + viewProj[3][1];

    // Top Plane
    planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
    planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
    planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
    planes[3].distance = viewProj[3][3] - viewProj[3][1];

    // Near Plane
    planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
    planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
    planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
    planes[4].distance = viewProj[3][3] + viewProj[3][2];

    // Far Plane
    planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
    planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
    planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
    planes[5].distance = viewProj[3][3] - viewProj[3][2];

    for (auto& plane : planes) {
      plane.Normalize();
    }
  }

  float velocity = moveSpeed * dt;
  if (ImGui::IsKeyDown(ImGuiKey_W)) camPos += camFront * velocity;
  if (ImGui::IsKeyDown(ImGuiKey_S)) camPos -= camFront * velocity;
  if (ImGui::IsKeyDown(ImGuiKey_A)) camPos -= glm::normalize(glm::cross(camFront, camUp)) * velocity;
  if (ImGui::IsKeyDown(ImGuiKey_D)) camPos += glm::normalize(glm::cross(camFront, camUp)) * velocity;
  if (ImGui::IsKeyDown(ImGuiKey_E)) camPos += camUp * velocity;
  if (ImGui::IsKeyDown(ImGuiKey_Q)) camPos -= camUp * velocity;
}


void ModelSample::FixedUpdate() {
}

bool ModelSample::IsOnFrustum(const glm::vec3& center, float radius) const {
  for (const auto& plane : planes) {
    // Calculate distance from center to plane
    // If distance < -radius, the sphere is completely behind this plane
    if (glm::dot(plane.normal, center) + plane.distance < -radius) {
      return false;
    }
  }
  return true;
}

void ModelSample::Draw() {
  //FramBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  pipeline.Bind();

  const auto size = common::GetWindowSize();
  view = glm::lookAt(camPos, camPos + camFront, camUp);
  proj = glm::perspective(glm::radians(fov),
                          static_cast<float>(size.x) / static_cast<float>(size.y), 0.1f, 100.f);

  pipeline.SetMat4("view", glm::value_ptr(view));
  pipeline.SetMat4("projection", glm::value_ptr(proj));
  pipeline.SetVec3("viewPos", camPos.x, camPos.y, camPos.z);

  // --- Draw Loop ---
  visibleModels.clear();
  for (auto &inst : instances) {
    if (IsOnFrustum(inst.position, 5)) {
      // 1. Set Per-Instance Uniforms
      //pipeline.SetMat4("model", glm::value_ptr(inst.model));
      //pipeline.SetFloat("opacity", inst.opacity);
      visibleModels.push_back(inst.model);
      // 2. Draw the Shared Model Data
    }
  }

  if (!visibleModels.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, visibleModels.size() * sizeof(glm::mat4), visibleModels.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    nanoSuitModel.Draw(pipeline, visibleModels.size());
  }

  glDepthFunc(GL_LEQUAL);
  skybox.Draw(view, proj);
  glDepthFunc(GL_LESS);

  //FramBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_DEPTH_TEST);

  //Dessiné le fram buffer
  //color atachmente
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
  glClear(GL_COLOR_BUFFER_BIT);

  screen_pipeline.Bind();
  screen_pipeline.SetInt("postProces", postProcess);
  glBindVertexArray(quadVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);	// use the color attachment texture as the texture of the quad plane
  glDrawArrays(GL_TRIANGLES, 0, 6);

}

void ModelSample::OnGui() {
  ImGui::Begin("Model Sample");
  ImGui::Text("Nanosuit Model");

  if (ImGui::CollapsingHeader("Camera")) {
    ImGui::DragFloat3("Pos", &camPos.x, 0.1f);
    ImGui::SliderFloat("FOV", &fov, 0.f, 150.0f);
  }

  if (ImGui::CollapsingHeader("Post Processing")) {
    const char *modes[] = {"Normal", "Inverse", "Grayscale", "Sharpen", "Blur", "Edge Detection"};
    if (ImGui::BeginCombo("Effect", modes[postProcess])) {
      for (int i = 0; i < 6; i++) {
        bool isSelected = (postProcess == i);
        if (ImGui::Selectable(modes[i], isSelected)) {
          postProcess = i;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }

  ImGui::End();
}
}