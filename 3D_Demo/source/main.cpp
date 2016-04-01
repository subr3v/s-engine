#include "SEngine.hpp"
#include "SponzaScene.h"

// Entry point. The program start here
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	SEngine::Init(hInstance, nCmdShow);
	SEngine::Run(new FSponzaScene());
	SEngine::Shutdown();
	return 0;
}
