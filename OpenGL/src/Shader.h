#pragma once

class OpenGL;

class Shader
{
protected:
	Shader();
	Shader(const Shader& other);

public:
	virtual ~Shader();

public:
	virtual bool Initialize(OpenGL* openGL, HWND) = 0;
	void Shutdown(OpenGL* openGL);
	void SetShader(OpenGL* openGL);
	virtual bool SetShaderParameters(OpenGL* openGL, 
									 float* worldMatrix, 
									 float* viewMatrix, 
									 float* projectionMatrix,
									 int textureUnit = 0,
									 float* lightDirection = nullptr, 
									 float* diffuseLightColour = nullptr,
									 float* ambientLight = nullptr);

protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* openGL, HWND hWnd);
	char* LoadShaderSourceFile(const char* filename);
	void OutputShaderErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int shaderId, const char* shaderFilename);
	void OutputLinkerErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int programId);
	void ShutdownShader(OpenGL* openGL);

protected:
	unsigned int _vertexShader;
	unsigned int _fragmentShader;
	unsigned int _shaderProgram;

};
