//
// Created by unite on 20.11.2025.
//

#include "engine/engine.h"
#include "engine/system.h"
#include "engine/window.h"
#include "gtest/gtest.h"

namespace test_gl {
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
}

TEST(GL, OpenGLESWindow) {
  test_gl::ClosingSystem closingSystem;
  auto windowConfig = common::GetWindowConfig();
  windowConfig.renderer = common::WindowConfig::RendererType::OPENGLES;
  common::SetWindowConfig(windowConfig);
  common::SystemObserverSubject::AddObserver(&closingSystem);
  common::RunEngine();
  common::SystemObserverSubject::RemoveObserver(&closingSystem);
}

TEST(GL, OpenGLWindow) {
  test_gl::ClosingSystem closingSystem;
  auto windowConfig = common::GetWindowConfig();
  windowConfig.renderer = common::WindowConfig::RendererType::OPENGL;
  common::SetWindowConfig(windowConfig);
  common::SystemObserverSubject::AddObserver(&closingSystem);
  common::RunEngine();
  common::SystemObserverSubject::RemoveObserver(&closingSystem);
}