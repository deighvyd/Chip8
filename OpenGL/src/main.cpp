#include "pch.h"

#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application& application = Application::GetInstance();
	if (application.Initialize())
	{
		application.Run();
	}

	application.Shutdown();
	
	// TODO - destroy the singleton

	return 0;
}