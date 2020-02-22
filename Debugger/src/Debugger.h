#pragma once

#include "Application.h"

class Debugger : public Application
{
public:
	Debugger();
	Debugger(Debugger const&) = delete;
    Debugger& operator=(Debugger const&) = delete;
	virtual ~Debugger();

	virtual void OnUpdate() override {}
	virtual void OnDraw() override {}

};

