#include "pch.h"

#include "Designer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Emulator* debugger = new Emulator();
	if (debugger->Initialize(L"Chip8 Designer"))
	{
		debugger->Run();
	}

	debugger->Shutdown();
	delete debugger;

	return 0;
}