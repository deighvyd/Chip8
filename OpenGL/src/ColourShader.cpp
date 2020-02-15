#include "pch.h"

#include "ColourShader.h"

#include "OpenGL.h"

using namespace std;

ColourShader::ColourShader()
	: Shader()
{
}

ColourShader::ColourShader(const ColourShader& other)
{
}

ColourShader::~ColourShader()
{
}

bool ColourShader::Initialize(OpenGL* openGL, HWND hWnd)
{
	if (!InitializeShader("shaders/Colour.vs", "shaders/Colour.ps", openGL, hWnd))
	{
		return false;
	}

	return true;
}


