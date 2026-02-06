  #include "5-cube_light.h"
#include "engine/engine.h"
  #include "engine/renderer.h"
  #include "engine/window.h"

  int main()
  {
	  auto windowConfig = common::GetWindowConfig();
  	windowConfig.renderer = common::WindowConfig::RendererType::OPENGLES;
  	SetWindowConfig(windowConfig);

  	graphics::CubeLight cube;


  	common::SystemObserverSubject::AddObserver(&cube);
  	common::DrawObserverSubject::AddObserver(&cube);

  	common::RunEngine();
  }