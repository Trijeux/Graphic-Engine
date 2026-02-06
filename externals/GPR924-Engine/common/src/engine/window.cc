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

#include "engine/window.h"

#include <GL/glew.h>

#include <format>

#include "engine/gui.h"
#include "maths/vec2.h"

namespace common {
namespace {
WindowConfig windowConfig;
SDL_Window* window = nullptr;
core::Vec2I windowSize;
float fixedDt;
bool isOpen = false;
}  // namespace
SDL_Window* GetWindow() { return window; }
void BeginWindow() {
  SDL_WindowFlags flags = 0;
  if (windowConfig.resizable) {
    flags |= SDL_WINDOW_RESIZABLE;
  }
  if (windowConfig.renderer == WindowConfig::RendererType::OPENGL ||
      windowConfig.renderer == WindowConfig::RendererType::OPENGLES) {
    flags |= SDL_WINDOW_OPENGL;
#if defined(__EMSCRIPTEN__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        windowConfig.renderer == WindowConfig::RendererType::OPENGLES
            ? SDL_GL_CONTEXT_PROFILE_ES
            : SDL_GL_CONTEXT_PROFILE_CORE);
    if (windowConfig.renderer == WindowConfig::RendererType::OPENGL) {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    } else {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_ES);

      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    }
#endif

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  }
  windowSize = {windowConfig.width, windowConfig.height};
  fixedDt = windowConfig.fixed_dt;
  window = SDL_CreateWindow(windowConfig.title.data(), windowConfig.width,
                            windowConfig.height, flags);
  if (window == nullptr) {
    throw std::runtime_error(
        std::format("Failed to create window, error: {}", SDL_GetError()));
  }
  isOpen = true;
}
void UpdateWindow() {
  SDL_Event e;
  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    // User requests quit
    if (e.type == SDL_EVENT_QUIT ||
        e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
      isOpen = false;
    } else if (e.type == SDL_EVENT_WINDOW_RESIZED) {
      windowSize = {e.window.data1, e.window.data2};
      if (windowConfig.renderer == WindowConfig::RendererType::OPENGL || windowConfig.renderer == WindowConfig::RendererType::OPENGLES) {
        glViewport(0, 0, windowSize.x, windowSize.y);
      }
    }
    ManageGuiEvent(e);
    for (auto* eventInterface : OnEventObserverSubject::GetObservers()) {
      eventInterface->OnEvent(e);
    }
  }
}
void EndWindow() { 
  SDL_DestroyWindow(window);
  window = nullptr;
}
core::Vec2I GetWindowSize() { return windowSize; }
float GetFixedDT() { return fixedDt; }
bool IsWindowOpen() { return isOpen; }
const WindowConfig& GetWindowConfig() { return windowConfig; }
void SetWindowConfig(const WindowConfig& config) { windowConfig = config; }
void CloseWindow() { isOpen = false; }
}  // namespace common