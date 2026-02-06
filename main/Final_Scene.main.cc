#include "FinalProject/Final_Scene.h"
#include "engine/engine.h"
#include "engine/window.h"

int main() {
  auto windowConfig = common::GetWindowConfig();
  windowConfig.renderer = common::WindowConfig::RendererType::OPENGL;
  windowConfig.resizable = true;
  SetWindowConfig(windowConfig);

  Graphics::Final_Scene sample;

  common::SystemObserverSubject::AddObserver(&sample);
  common::DrawObserverSubject::AddObserver(&sample);
  common::OnGuiObserverSubject::AddObserver(&sample);

  common::RunEngine();
}