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

	GLuint GetShaderHandle() const { return _shaderProgram; }

protected:
	virtual void BindAttributes(OpenGL* openGL) const;

	bool LoadShader(const char* vsFilename, const char* fsFilename, OpenGL* openGL, HWND hWnd);
	bool CompileShader(const char* vsSource, const char* fsSource, OpenGL* openGL, HWND hWnd);
	char* LoadShaderSourceFile(const char* filename);
	void OutputShaderErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int shaderId, const char* shaderFilename);
	void OutputLinkerErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int programId);
	void ShutdownShader(OpenGL* openGL);

public:
	template<class ShaderType>
	static bool InitShader(Shader*& shader, OpenGL* openGL, HWND hWnd);
	void DestroyShader(Shader*& shader, OpenGL* openGL);

protected:
	GLuint _vertexShader;
	GLuint _fragmentShader;
	GLuint _shaderProgram;

};

template<class ShaderType>
bool Shader::InitShader(Shader*& shader, OpenGL* openGL, HWND hWnd)
{
	if ((shader = new ShaderType()) == nullptr)
	{
		return false;
	}

	if (!shader->Initialize(openGL, hWnd))
	{
		return false;
	}

	return true;
}
