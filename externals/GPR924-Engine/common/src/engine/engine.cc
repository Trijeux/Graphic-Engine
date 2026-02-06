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
Contributors: Anthony Barman
*/

#include "engine/engine.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>

#include <format>

#include "engine/renderer.h"
#include "engine/window.h"
#include "third_party/sdl3_include.h"

#ifdef _WIN32
    #include <windows.h>

    #ifdef __cplusplus
    extern "C" {
#endif

      // Force l'utilisation du GPU dédié NVIDIA
      __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

      // Force l'utilisation du GPU dédié AMD
      __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
    }
#endif
#endif

namespace common {
namespace {
float dt = 0.0f;
Uint64 currentTime = 0u;
}  // namespace
static void BeginEngine() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    throw std::runtime_error(
        std::format("SDL failed to initialise: {}", SDL_GetError()));
  }
  BeginWindow();
  BeginRenderer();
  for (auto* system : SystemObserverSubject::GetObservers()) {
    if (system == nullptr) continue;
    system->Begin();
  }
}
static void EndEngine() {
  for (auto* system : SystemObserverSubject::GetObservers()) {
    if (system == nullptr) continue;
    system->End();
  }
  EndRenderer();
  EndWindow();

  SDL_Quit();
}
void RunEngine() {
  BeginEngine();
  auto freq = static_cast<double>(SDL_GetPerformanceFrequency());
  Uint64 previous = SDL_GetPerformanceCounter();
  float dt_fix_ = 0;
  while (IsWindowOpen()) {
    currentTime = SDL_GetPerformanceCounter();
    auto delta = static_cast<double>(currentTime - previous);
    previous = currentTime;
    dt = static_cast<float>(delta / freq);

    UpdateWindow();
    UpdateRenderer();

    for (auto& system : SystemObserverSubject::GetObservers()) {
      if (system == nullptr) continue;
      system->Update(dt);
    }

    dt_fix_ += dt;
    while (dt_fix_ >= GetFixedDT()) {
      for (auto& system : SystemObserverSubject::GetObservers()) {
        if (!IsWindowOpen()) break;  // <-- arrêt immédiat si la fenêtre vient d’être fermée
        if (system == nullptr) continue;
        system->FixedUpdate();
      }

      if (!IsWindowOpen()) break; // <-- sécurité supplémentaire après la boucle interne
      dt_fix_ -= GetFixedDT();
    }

    DrawRenderer();
  }
  EndEngine();
}
float GetDeltaTime() { return dt; }
}  // namespace common