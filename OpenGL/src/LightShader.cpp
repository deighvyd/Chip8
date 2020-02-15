#include "pch.h"

#include "LightShader.h"

#include "OpenGL.h"

using namespace std;

LightShader::LightShader()
	: TextureShader()
{
}

LightShader::LightShader(const LightShader&)
{
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(OpenGL* openGL, HWND hWnd)
{
	if (!InitializeShader("shaders/Light.vs", "shaders/Light.ps", openGL, hWnd))
	{
		return false;
	}

	return true;
}

bool LightShader::SetShaderParameters(OpenGL* openGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, float* lightDirection, float* diffuseLightColour)
{
	if (!TextureShader::SetShaderParameters(openGL, worldMatrix, viewMatrix, projectionMatrix, textureUnit))
	{
		return false;
	}

	// set the light direction and colour
	unsigned int location = openGL->glGetUniformLocation(_shaderProgram, "lightDirection");
	if (location == -1)
	{
		return false;
	}
	openGL->glUniform3fv(location, 1, lightDirection);

	location = openGL->glGetUniformLocation(_shaderProgram, "diffuseLightColour");
	if (location == -1)
	{
		return false;
	}
	openGL->glUniform4fv(location, 1, diffuseLightColour);
	
	return true;
}