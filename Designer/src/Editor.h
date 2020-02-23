#pragma once

#include "Application.h"

class Editor : public Application
{
public:
	Editor();
	Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;
	virtual ~Editor();

	virtual void OnUpdate() override {}
	virtual void OnDraw() override {}

};

