#pragma once

#include "OpenGL.h"

class ColourShader
{
public:
	ColourShader();
	ColourShader(const ColourShader&);
	~ColourShader();

	bool Initialize(OpenGL* openGL, HWND hWnd);
	void Shutdown(OpenGL* openGL);
	void SetShader(OpenGL* openGL);
	bool SetShaderParameters(OpenGL* openGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix);

private:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* openGL, HWND hWnd);
	char* LoadShaderSourceFile(const char* filename);
	void OutputShaderErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int shaderId, const char* shaderFilename);
	void OutputLinkerErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int programId);
	void ShutdownShader(OpenGL* openGL);

private:
	unsigned int _vertexShader;
	unsigned int _fragmentShader;
	unsigned int _shaderProgram;
};

