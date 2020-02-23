#include "pch.h"

#include "Designer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Designer* debugger = new Designer();
	if (debugger->Initialize(L"Chip8 Editor"))
	{
		debugger->Run();
	}

	debugger->Shutdown();
	delete debugger;

	return 0;
}