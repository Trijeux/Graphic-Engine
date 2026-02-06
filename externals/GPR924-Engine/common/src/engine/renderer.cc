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
Contributors: Anthony Barman, Jemoel Ablay
*/

#include "engine/renderer.h"

#include <cmath>
#include <format>

#include "engine/gui.h"
#include "engine/window.h"
#include "maths/constant.h"
#include "third_party/gl_include.h"

namespace common {

namespace {
SDL_Renderer* renderer = nullptr;
SDL_GLContext gl_context;
}  // namespace
void DrawInterface::PreDraw() {}
void DrawInterface::PostDraw() {}
void BeginRenderer() {
  auto* window = GetWindow();

  const auto& windowConfig = GetWindowConfig();

  switch (windowConfig.renderer) {
    case WindowConfig::RendererType::SDL_RENDERER: {
      renderer = SDL_CreateRenderer(window, nullptr);
      if (renderer == nullptr) {
        throw std::runtime_error(std::format(
            "SDL renderer failed to initialise: {}", SDL_GetError()));
      }
      break;
    }
    case WindowConfig::RendererType::OPENGL:
    case WindowConfig::RendererType::OPENGLES: {
      gl_context = SDL_GL_CreateContext(GetWindow());
      if (!gl_context) {
        throw std::runtime_error(std::format(
            "Could not create OpenGL context, error: {}", SDL_GetError()));
      }
      SDL_GL_SetSwapInterval(1);

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
      GLenum err = glewInit();
      if (GLEW_OK != err) {
        throw std::runtime_error(std::format("GLEW Init failed with error code: {}", err));
      }
#endif
      break;
    }
  }

  BeginGuiRenderer();
}
void UpdateRenderer() { UpdateGuiRenderer(); }
void DrawRenderer() {
  const auto& windowConfig = GetWindowConfig();
  switch (windowConfig.renderer) {
    case WindowConfig::RendererType::SDL_RENDERER: {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);

      for (auto& drawInterface : DrawObserverSubject::GetObservers()) {
        drawInterface->PreDraw();
      }

      for (auto& drawInterface : DrawObserverSubject::GetObservers()) {
        drawInterface->Draw();
      }

      for (auto& drawInterface : DrawObserverSubject::GetObservers()) {
        drawInterface->PostDraw();
      }
      DrawGuiRenderer();

      SDL_RenderPresent(renderer);
      break;
    }
    case WindowConfig::RendererType::OPENGL:
    case WindowConfig::RendererType::OPENGLES: {
      glClear(GL_COLOR_BUFFER_BIT);
      for (auto& drawInterface : DrawObserverSubject::GetObservers()) {
        if (drawInterface == nullptr) {
          continue;
        }
        drawInterface->PreDraw();
      }

      for (auto& drawInterface : DrawObserverSubject::GetObservers()) {
        if (drawInterface == nullptr) {
          continue;
        }
        drawInterface->Draw();
      }

      for (auto& drawInterface : DrawObserverSubject::GetObservers()) {
        if (drawInterface == nullptr) {
          continue;
        }
        drawInterface->PostDraw();
      }
      DrawGuiRenderer();
      SDL_GL_SwapWindow(GetWindow());
      break;
    }
  }
}
void EndRenderer() {
  EndGuiRenderer();
  auto& windowConfig = GetWindowConfig();
  if (windowConfig.renderer == WindowConfig::RendererType::SDL_RENDERER) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  } else {
    SDL_GL_DestroyContext(gl_context);
    gl_context = nullptr;
  }
}

void DrawCircle(const float centerX, const float centerY, const float radius,
                const SDL_FColor color, const int nbSegments) {
  for (int i = 0; i < nbSegments; ++i) {
    const float angle1 =
        2 * core::PI * static_cast<float>(i) / static_cast<float>(nbSegments);
    const float angle2 = 2 * core::PI * static_cast<float>(i + 1) /
                         static_cast<float>(nbSegments);

    const float x1 =
        centerX + radius * static_cast<float>(cos(static_cast<double>(angle1)));
    const float y1 =
        centerY + radius * static_cast<float>(sin(static_cast<double>(angle1)));
    const float x2 =
        centerX + radius * static_cast<float>(cos(static_cast<double>(angle2)));
    const float y2 =
        centerY + radius * static_cast<float>(sin(static_cast<double>(angle2)));

    SDL_Vertex vertices[3];
    vertices[0].position = {centerX, centerY};
    vertices[0].color = color;
    vertices[1].position = {x1, y1};
    vertices[1].color = color;
    vertices[2].position = {x2, y2};
    vertices[2].color = color;

    SDL_RenderGeometry(common::GetRenderer(), nullptr, vertices, 3, nullptr, 0);
  }
}

void DrawAABB(const core::Vec2F pos, const core::Vec2F rectSize,
              const SDL_FColor color, const bool filled) {
  const SDL_FRect rect(pos.x - rectSize.x / 2, pos.y - rectSize.y / 2,
                       rectSize.x, rectSize.y);

  SDL_SetRenderDrawColorFloat(GetRenderer(), color.r, color.g, color.b,
                              color.a);
  if (filled)
    SDL_RenderFillRect(GetRenderer(), &rect);
  else
    SDL_RenderRect(GetRenderer(), &rect);
}

SDL_Renderer* GetRenderer() { return renderer; }
SDL_GLContext GetGlContext() { return gl_context; }
}  // namespace common