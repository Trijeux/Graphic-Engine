/*
Copyright 2025 SAE Institute Switzerland SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Contributors: Elias Farhan
*/

#include "engine/gui.h"

#include "engine/renderer.h"
#include "engine/window.h"
#include "third_party/imgui_include.h"

namespace common {
namespace {
SDL_Renderer* renderer_;
}
void BeginGuiRenderer() {
  renderer_ = GetRenderer();
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  style.ScaleAllSizes(
      mainScale);  // Bake a fixed style scale. (until we have a solution for
                   // dynamic style scaling, changing this requires resetting
                   // Style + calling this again)
  const auto& windowConfig = GetWindowConfig();
  if (windowConfig.renderer == WindowConfig::RendererType::SDL_RENDERER) {
    ImGui_ImplSDL3_InitForSDLRenderer(GetWindow(), renderer_);
    ImGui_ImplSDLRenderer3_Init(renderer_);
  }
  else {
    ImGui_ImplSDL3_InitForOpenGL(GetWindow(), GetGlContext());
    ImGui_ImplOpenGL3_Init("#version 300 es");
  }
}
void UpdateGuiRenderer() {
  auto& windowConfig = GetWindowConfig();
  if (windowConfig.renderer == WindowConfig::RendererType::SDL_RENDERER) {
    ImGui_ImplSDLRenderer3_NewFrame();
  }
  else {
    ImGui_ImplOpenGL3_NewFrame();
  }
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  for (auto* guiInterface : OnGuiObserverSubject::GetObservers()) {
    if (guiInterface == nullptr) continue;
    guiInterface->OnGui();
  }
}
void EndGuiRenderer() {
  auto& windowConfig = GetWindowConfig();
  if (windowConfig.renderer == WindowConfig::RendererType::SDL_RENDERER) {
    ImGui_ImplSDLRenderer3_Shutdown();
  }
  else {
    ImGui_ImplOpenGL3_Shutdown();
  }
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
void DrawGuiRenderer() {
  ImGui::Render();
  ImGuiIO& io = ImGui::GetIO();
  SDL_SetRenderScale(renderer_, io.DisplayFramebufferScale.x,
                     io.DisplayFramebufferScale.y);
  auto& windowConfig = GetWindowConfig();
  if (windowConfig.renderer == WindowConfig::RendererType::SDL_RENDERER) {
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
  }
  else {
    auto* draw_data = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(draw_data);
  }
}
void ManageGuiEvent(const SDL_Event& event) {
  ImGui_ImplSDL3_ProcessEvent(&event);
}
}  // namespace common