#pragma once

#include "Application.h"

class Designer : public Application
{
public:
	Designer();
	Designer(Designer const&) = delete;
    Designer& operator=(Designer const&) = delete;
	virtual ~Designer();

	virtual void OnUpdate() override {}
	virtual void OnDraw() override {}

};

