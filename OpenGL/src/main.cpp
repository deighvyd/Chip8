#include "pch.h"

#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application* application = new Application();
	if (application == nullptr)
	{
		return 1;
	}
	
	if (application->Initialize())
	{
		application->Run();
	}

	application->Shutdown();
	delete application;
	application = nullptr;

	return 0;
}