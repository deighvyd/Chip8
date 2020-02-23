#pragma once

#include "Application.h"

class Emulator : public Application
{
public:
	Emulator();
	Emulator(Emulator const&) = delete;
    Emulator& operator=(Emulator const&) = delete;
	virtual ~Emulator();

	virtual void OnUpdate() override {}
	virtual void OnDraw() override {}

};

