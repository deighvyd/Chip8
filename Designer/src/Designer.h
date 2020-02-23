#pragma once

#include "Application.h"

class Chip8;

class Designer : public Application
{
public:
	Designer();
	Designer(Designer const&) = delete;
    Designer& operator=(Designer const&) = delete;
	virtual ~Designer();

	virtual void OnUpdate() override {}
	virtual void OnDraw() override {}
	virtual void OnGui() override;

private:
	Chip8* _chip8 = nullptr;

	const char* _programFile;
	size_t _programSize = 0;
	unsigned char* _program = nullptr;		// store our own copy of the program so we don't have to pollute the emulator

};
