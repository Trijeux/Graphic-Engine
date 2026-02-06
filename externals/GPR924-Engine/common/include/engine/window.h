#ifndef GPR924_ENGINE_WINDOW_H
#define GPR924_ENGINE_WINDOW_H

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

#include "maths/vec2.h"
#include "third_party/sdl3_include.h"
#include "utils/observer.h"

namespace common {
class OnEventInterface {
 public:
  virtual ~OnEventInterface() = default;
  virtual void OnEvent(const SDL_Event& event) = 0;
};



struct WindowConfig {
  enum class RendererType: uint8_t {
    SDL_RENDERER,
    OPENGLES,
    OPENGL
  };
  std::string title = "GPR924-Engine";
  int width = 1280;
  int height = 720;
  float fixed_dt = 0.016f;
  bool fullscreen = false;
  bool resizable = false;
  RendererType renderer = RendererType::SDL_RENDERER;
};

using OnEventObserverSubject = ObserverSubject<OnEventInterface>;

[[nodiscard]] SDL_Window* GetWindow();

void BeginWindow();
void UpdateWindow();
void EndWindow();
[[nodiscard]] core::Vec2I GetWindowSize();
float GetFixedDT();
bool IsWindowOpen();
void SetWindowConfig(const WindowConfig& config);
[[nodiscard]] const WindowConfig& GetWindowConfig();
void CloseWindow();
}  // namespace common
#endif  // GPR924_ENGINE_WINDOW_H
