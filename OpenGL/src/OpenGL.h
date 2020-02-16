#pragma once

class OpenGL
{
public:
	OpenGL();
	OpenGL(const OpenGL&);
	virtual ~OpenGL();

	bool InitializeExtensions(HWND hWnd);
	bool InitializeOpenGL(HWND hWnd, int screenWidth, int screenHeight, float screenDepth, float screenNear, bool vSync);
	void Shutdown(HWND hWnd);

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	void GetWorldMatrix(float *matrix);
	void GetProjectionMatrix(float *matrix);
	void GetVideoCardInfo(char *cardName);

	void BuildIdentityMatrix(float *matrix);
	void BuildPerspectiveFovLHMatrix(float *matrix, float fieldOfView, float screenAspect, float screenNear, float screenDepth);
	void MatrixRotationY(float *matrix, float angle);
	void MatrixTranslation(float *matrix, float x, float y, float z);
	void MatrixMultiply(float *result, float *matrix1, float *matrix2);

private:
	bool LoadExtensionList();

private:
	HDC _deviceContext;
	HGLRC _renderingContext;

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

	float _worldMatrix[16];
	float _projectionMatrix[16];
	char _videoCardDescription[128];

public:
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBINDSAMPLERPROC glBindSampler;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	PFNGLBLENDEQUATIONPROC glBlendEquation;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	PFNGLDETACHSHADERPROC glDetachShader;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	PFNGLGETPROGRAMIVPROC glGetProgramiv;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLGETSHADERIVPROC glGetShaderiv;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
	PFNGLACTIVETEXTUREPROC glActiveTexture;
	PFNGLUNIFORM1IPROC glUniform1i;
	PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
	PFNGLUNIFORM3FVPROC glUniform3fv;
	PFNGLUNIFORM4FVPROC glUniform4fv;

};
