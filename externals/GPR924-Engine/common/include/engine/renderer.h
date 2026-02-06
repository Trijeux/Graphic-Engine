#ifndef GPR924_ENGINE_RENDERER_H
#define GPR924_ENGINE_RENDERER_H

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

#include "maths/vec2.h"
#include "third_party/sdl3_include.h"
#include "utils/observer.h"

namespace common {
class DrawInterface
{
 public:
  virtual ~DrawInterface() = default;
  virtual void PreDraw();
  virtual void Draw() = 0;
  virtual void PostDraw();
};

using DrawObserverSubject = ObserverSubject<DrawInterface>;

void BeginRenderer();
void UpdateRenderer();
void DrawRenderer();
void EndRenderer();
void DrawCircle(float centerX, float centerY, float radius,
                SDL_FColor color, int nbSegments = 20);
void DrawAABB(core::Vec2F pos, core::Vec2F rectSize, SDL_FColor color, bool filled = true);


SDL_Renderer* GetRenderer();
SDL_GLContext GetGlContext();
}

#endif  // GPR924_ENGINE_RENDERER_H
