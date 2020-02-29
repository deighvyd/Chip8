#pragma once

#include "Application.h"

class Emulator : public Application
{
public:
	static constexpr int Scale = 10;

public:
	Emulator();
	Emulator(Emulator const&) = delete;
    Emulator& operator=(Emulator const&) = delete;
	virtual ~Emulator();

	virtual void OnUpdate(float delta) override {}
	virtual void OnDraw() override {}
	virtual void OnGui() override {}

};

