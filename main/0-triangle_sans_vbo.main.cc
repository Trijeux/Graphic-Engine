#include "0-triangle_sans_vbo.h"
#include "engine/engine.h"
#include "engine/window.h"

int main()
{
	auto windowConfig = common::GetWindowConfig();
	windowConfig.renderer = common::WindowConfig::RendererType::OPENGLES;
	SetWindowConfig(windowConfig);

	graphics::TriangleSansVbo triangle;

	common::SystemObserverSubject::AddObserver(&triangle);
	common::DrawObserverSubject::AddObserver(&triangle);

	common::RunEngine();
}
