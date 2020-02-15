#pragma once

#include "Shader.h"

class TextureShader : public Shader
{
public:
	TextureShader();
	TextureShader(const TextureShader& other);
	virtual ~TextureShader();

	virtual bool Initialize(OpenGL* openGL, HWND hWnd) override;
	virtual bool SetShaderParameters(OpenGL* openGL, 
									 float* worldMatrix, 
									 float* viewMatrix, 
									 float* projectionMatrix, 
									 int textureUnit = 0) override;
	
};
