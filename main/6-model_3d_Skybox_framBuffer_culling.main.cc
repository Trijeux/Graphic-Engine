#include "6-model_sample.h"
#include "engine/engine.h"
#include "engine/window.h"

int main() {
    auto windowConfig = common::GetWindowConfig();
    windowConfig.renderer = common::WindowConfig::RendererType::OPENGLES;
    windowConfig.resizable = true;
    SetWindowConfig(windowConfig);

    graphics::ModelSample sample;

    common::SystemObserverSubject::AddObserver(&sample);
    common::DrawObserverSubject::AddObserver(&sample);
    common::OnGuiObserverSubject::AddObserver(&sample);

    common::RunEngine();
}