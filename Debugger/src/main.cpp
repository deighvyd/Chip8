#include "pch.h"

#include "Debugger.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Debugger* debugger = new Debugger();
	if (debugger->Initialize(L"Chip8 Editor"))
	{
		debugger->Run();
	}

	debugger->Shutdown();
	delete debugger;

	return 0;
}