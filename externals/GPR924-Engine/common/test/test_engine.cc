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

#include <imgui.h>

#include "engine/engine.h"
#include "engine/gui.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "gtest/gtest.h"

#include "third_party/sdl3_include.h"

class ClosingSystem : public common::SystemInterface {
 public:
  void Begin() override {}
  void End() override {}
  void Update([[maybe_unused]] float dt) override {
    ++count_;
    if (count_ > kCloseCount) {
      common::CloseWindow();
    }
  }
  void FixedUpdate() override {
    ++count_;
    if (count_ > kCloseCount) {
      common::CloseWindow();
    }
  }

 protected:
  static constexpr int kCloseCount = 100;
  int count_ = 0;
};

TEST(Engine, RunEngine) {
  ClosingSystem closingSystem;
  common::SystemObserverSubject::AddObserver(&closingSystem);
  common::RunEngine();
  common::SystemObserverSubject::RemoveObserver(&closingSystem);
}

class ClosingSystemWithGui : public ClosingSystem,
                             public common::OnGuiInterface {
 public:
  void OnGui() override {
    ImGui::Begin("New Gui Window");
    ImGui::Text("Count: %d", count_);
    ImGui::End();
  }
};

TEST(Engine, Gui) {
  ClosingSystemWithGui closingSystem;
  common::SystemObserverSubject::AddObserver(&closingSystem);
  common::OnGuiObserverSubject::AddObserver(&closingSystem);
  common::RunEngine();

  common::OnGuiObserverSubject::RemoveObserver(&closingSystem);
  common::SystemObserverSubject::RemoveObserver(&closingSystem);
}

class DrawSomething : public common::DrawInterface {
public:
  void Draw() override {
    auto* renderer = common::GetRenderer();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FRect rect{.x = 100, .y = 100, .w = 200, .h = 200};
    SDL_RenderFillRect(renderer, &rect);
  }
};


TEST(Engine, Render) {
  DrawSomething draw_something;
  common::DrawObserverSubject::AddObserver(&draw_something);
  common::RunEngine();
  common::DrawObserverSubject::RemoveObserver(&draw_something);
}

class Circle : public common::DrawInterface
{
  void Draw() override {
    SDL_RenderClear(common::GetRenderer());

    common::DrawCircle(400, 300, 40, {255, 255, 0, 255}); // Soleil
    common::DrawCircle(550, 300, 10, {0, 0, 255, 255}); // Planète

  }
};

TEST(Render, Circle)
{
  Circle circle;
  common::DrawObserverSubject::AddObserver(&circle);
  common::RunEngine();
  common::DrawObserverSubject::RemoveObserver(&circle);
}


class AABB : public common::DrawInterface
{
  void Draw() override {
    SDL_RenderClear(common::GetRenderer());

    common::DrawAABB({50,50}, {10,10}, {255, 255, 0, 255});
  }
};

TEST(Render, AABB)
{
  AABB aabb;
  common::DrawObserverSubject::AddObserver(&aabb);
  common::RunEngine();
  common::DrawObserverSubject::RemoveObserver(&aabb);
}