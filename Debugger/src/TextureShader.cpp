#include "pch.h"

#include "TextureShader.h"

#include "OpenGL.h"

using namespace std;

TextureShader::TextureShader()
	: Shader()
{
}

TextureShader::TextureShader(const TextureShader&)
{
}

TextureShader::~TextureShader()
{
}

bool TextureShader::Initialize(OpenGL* openGL, HWND hWnd)
{
	if (!LoadShader("shaders/texture.vs", "shaders/texture.ps", openGL, hWnd))
	{
		return false;
	}

	return true;
}

bool TextureShader::SetShaderParameters(OpenGL* openGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, float* lightDirection, float* diffuseLightColour, float* ambientLight)
{
	if (!Shader::SetShaderParameters(openGL, worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	unsigned int location = openGL->glGetUniformLocation(_shaderProgram, "shaderTexture");
	if (location == -1)
	{
		return false;
	}
	openGL->glUniform1i(location, textureUnit);
	
	return true;
}