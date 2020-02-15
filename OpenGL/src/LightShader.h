#pragma once

#include "TextureShader.h"

class LightShader : public TextureShader
{
public:
	LightShader();
	LightShader(const LightShader& other);
	virtual ~LightShader();

	virtual bool Initialize(OpenGL* openGL, HWND hWnd) override;
	virtual bool SetShaderParameters(OpenGL* openGL, 
									 float* worldMatrix, 
									 float* viewMatrix, 
									 float* projectionMatrix, 
									 int textureUnit = 0,
									 float* lightDirection = nullptr, 
									 float* diffuseLightColour = nullptr) override;
	
};
