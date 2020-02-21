#include "pch.h"

#include "Input.h"
#include "Log.h"

Input::Input()
	: _charHandler(nullptr)
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
	if (key < NumKeys)
	{
		_keys[key] = true;
		//logging::Info("Key %d down", key);
	}
}

void Input::KeyUp(unsigned int key)
{
	if (key < NumKeys)
	{
		_keys[key] = false;
		//logging::Info("Key %d up", key);
	}
}

bool Input::IsKeyDown(unsigned int key)
{
	if (key < NumKeys)
	{
		return _keys[key];
	}

	return false;
}

void Input::OnCharacterPressed(unsigned int c)
{
	if (_charHandler != nullptr)
	{
		_charHandler(c);
	}
}

bool Input::IsButtonPressed(unsigned int button) const
{
	if (button < NumMouseButtons)
	{
		return _mouseButtons[button]; 
	}

	return false;
}

void Input::OnMouseDown(unsigned int button)
{
	if (button < NumMouseButtons)
	{
		_mouseButtons[button] = true;
	}
}

void Input::OnMouseUp(unsigned int button)
{
	if (button < NumMouseButtons)
	{
		_mouseButtons[button] = false;
	}
}

void Input::OnMouseMove(int x, int y)
{
	_mousePos.x = x;
	_mousePos.y = y;
}

void Input::ClearButtons()
{
	for (int i = 0 ; i < NumMouseButtons ; ++i)
	{
		_mouseButtons[i] = false;
	}
}