#include "3-square_avec_texture.h"
#include "engine/engine.h"
#include "engine/renderer.h"
#include "engine/window.h"

int main()
{
	auto windowConfig = common::GetWindowConfig();
	windowConfig.renderer = common::WindowConfig::RendererType::OPENGLES;
	SetWindowConfig(windowConfig);

	graphics::SquareAvecTexture square;

	common::SystemObserverSubject::AddObserver(&square);
	common::DrawObserverSubject::AddObserver(&square);

	common::RunEngine();
}
