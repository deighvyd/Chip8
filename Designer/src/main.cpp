#include "pch.h"

#include "Designer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Designer* designer = new Designer();
	if (designer->Initialize(L"Chip8 Designer"))
	{
		designer->Run();
	}

	designer->Shutdown();
	delete designer;

	return 0;
}