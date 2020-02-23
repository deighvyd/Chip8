#include "pch.h"

#include "Emulator.h"

#include "Chip8.h"

Emulator::Emulator()
	: Application(Scale * Chip8::ScreenWidth, Scale * Chip8::ScreenHeight)
{
}

Emulator::~Emulator()
{
}
