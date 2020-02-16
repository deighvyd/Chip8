#pragma once

#include "Shader.h"

class ColourShader : public Shader
{
public:
	ColourShader();
	ColourShader(const ColourShader& other);
	virtual ~ColourShader();

	virtual bool Initialize(OpenGL* openGL, HWND hWnd) override;
	
};

