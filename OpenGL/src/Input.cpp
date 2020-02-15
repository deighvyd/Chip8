#include "pch.h"

#include "Input.h"

Input::Input()
{
}

Input::Input(const Input& other)
{
}

Input::~Input()
{
}

void Input::Initialize()
{
	for (unsigned int key = 0; key < NumKeys ; key++)
	{
		_keys[key] = false;
	}
}

void Input::KeyDown(unsigned int key)
{
	_keys[key] = true;
}

void Input::KeyUp(unsigned int key)
{
	_keys[key] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	return _keys[key];
}
