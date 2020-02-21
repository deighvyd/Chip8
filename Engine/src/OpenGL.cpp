#include "pch.h"

#include "OpenGL.h"

OpenGL::OpenGL()
	: _deviceContext(nullptr)
	, _renderingContext(nullptr)
{
}

OpenGL::OpenGL(const OpenGL& other)
{
}

OpenGL::~OpenGL()
{
}

bool OpenGL::InitializeExtensions(HWND hWnd)
{
	HDC deviceContext;
	if ((deviceContext = GetDC(hWnd)) == nullptr)
	{
		return false;
	}

	// set a temporary default pixel format, rendering context
	PIXELFORMATDESCRIPTOR pixelFormat;
	int error = SetPixelFormat(deviceContext, 1, &pixelFormat);
	if (error != 1)
	{
		return false;
	}

	HGLRC renderContext = wglCreateContext(deviceContext);
	if (renderContext == nullptr)
	{
		return false;
	}

	error = wglMakeCurrent(deviceContext, renderContext);
	if (error != 1)
	{
		return false;
	}

	// initialize the OpenGL extensions needed for this application.  
	// Note that a temporary rendering context was needed to do so.
	bool result = LoadExtensionList();
	if (!result)
	{
		return false;
	}

	// cleanup
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(renderContext);
	renderContext = nullptr;

	ReleaseDC(hWnd, deviceContext);
	deviceContext = nullptr;

	return true;
}

bool OpenGL::InitializeOpenGL(HWND hWnd, int screenWidth, int screenHeight, float screenDepth, float screenNear, bool vSync)
{
	// get the device context for this window.
	_deviceContext = GetDC(hWnd);
	if (_deviceContext == nullptr)
	{
		return false;
	}
	
	int attributeListInt[19];
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;
	attributeListInt[18] = 0;

	// query for a pixel format that fits the attributes we want.
	int pixelFormat[1];
	unsigned int formatCount;
	int result = wglChoosePixelFormatARB(_deviceContext, attributeListInt, nullptr, 1, pixelFormat, &formatCount);
	if (result != 1)
	{
		return false;
	}

	// if the video card/display can handle our desired pixel format then we set it as the current one.
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	result = SetPixelFormat(_deviceContext, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1)
	{
		return false;
	}

	// set the 4.0 version of OpenGL in the attribute list.
	int attributeList[5];
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;
	attributeList[4] = 0;

	_renderingContext = wglCreateContextAttribsARB(_deviceContext, 0, attributeList);
	if (_renderingContext == nullptr)
	{
		return false;
	}

	result = wglMakeCurrent(_deviceContext, _renderingContext);
	if (result != 1)
	{
		return false;
	}
	
	glClearDepth(1.0f);
	BuildIdentityMatrix(_worldMatrix);

	// set the field of view and screen aspect ratio
	float fieldOfView = 3.14159265358979323846f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	BuildPerspectiveFovLHMatrix(_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	char *vendorString = (char*)glGetString(GL_VENDOR);
	char *rendererString = (char*)glGetString(GL_RENDERER);

	// Store the video card name in a class member variable so it can be retrieved later.
	strcpy_s(_videoCardDescription, vendorString);
	strcat_s(_videoCardDescription, " - ");
	strcat_s(_videoCardDescription, rendererString);

	// Turn on or off the vertical sync depending on the input bool value.
	if (vSync)
	{
		result = wglSwapIntervalEXT(1);
	}
	else
	{
		result = wglSwapIntervalEXT(0);
	}

	if (result != 1)
	{
		return false;
	}

	return true;
}

void OpenGL::Shutdown(HWND hWnd)
{
	if (_renderingContext != nullptr)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(_renderingContext);
		_renderingContext = nullptr;
	}

	if (_deviceContext != nullptr)
	{
		ReleaseDC(hWnd, _deviceContext);
		_deviceContext = nullptr;
	}
}

void OpenGL::BeginScene(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::EndScene()
{
	SwapBuffers(_deviceContext);
}

bool OpenGL::LoadExtensionList()
{
#define BIND_GL_FUNCTION(name, type)		\
	name = (type)wglGetProcAddress(#name);	\
	if (name == nullptr)					\
	{										\
		return false;						\
	}

	// load the OpenGL extensions that this application will be using.
	BIND_GL_FUNCTION(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC)
	BIND_GL_FUNCTION(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC)
	BIND_GL_FUNCTION(wglSwapIntervalEXT, PFNWGLSWAPINTERVALEXTPROC)

	BIND_GL_FUNCTION(glAttachShader, PFNGLATTACHSHADERPROC)
	BIND_GL_FUNCTION(glBindBuffer, PFNGLBINDBUFFERPROC)
	BIND_GL_FUNCTION(glBindSampler, PFNGLBINDSAMPLERPROC)
	BIND_GL_FUNCTION(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC)
	BIND_GL_FUNCTION(glBlendEquation, PFNGLBLENDEQUATIONPROC)
	BIND_GL_FUNCTION(glBufferData, PFNGLBUFFERDATAPROC)
	BIND_GL_FUNCTION(glCompileShader, PFNGLCOMPILESHADERPROC)
	BIND_GL_FUNCTION(glCreateProgram, PFNGLCREATEPROGRAMPROC)
	BIND_GL_FUNCTION(glCreateShader, PFNGLCREATESHADERPROC)
	BIND_GL_FUNCTION(glDeleteBuffers, PFNGLDELETEBUFFERSPROC)
	BIND_GL_FUNCTION(glDeleteProgram, PFNGLDELETEPROGRAMPROC)
	BIND_GL_FUNCTION(glDeleteShader, PFNGLDELETESHADERPROC)
	BIND_GL_FUNCTION(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC)
	BIND_GL_FUNCTION(glDetachShader, PFNGLDETACHSHADERPROC)
	BIND_GL_FUNCTION(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC)
	BIND_GL_FUNCTION(glGenBuffers, PFNGLGENBUFFERSPROC)
	BIND_GL_FUNCTION(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC)
	BIND_GL_FUNCTION(glGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC)
	BIND_GL_FUNCTION(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC)
	BIND_GL_FUNCTION(glGetProgramiv, PFNGLGETPROGRAMIVPROC)
	BIND_GL_FUNCTION(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC)
	BIND_GL_FUNCTION(glGetShaderiv, PFNGLGETSHADERIVPROC)
	BIND_GL_FUNCTION(glLinkProgram, PFNGLLINKPROGRAMPROC)
	BIND_GL_FUNCTION(glShaderSource, PFNGLSHADERSOURCEPROC)
	BIND_GL_FUNCTION(glUseProgram, PFNGLUSEPROGRAMPROC)
	BIND_GL_FUNCTION(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC)
	BIND_GL_FUNCTION(glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC)
	BIND_GL_FUNCTION(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC)
	BIND_GL_FUNCTION(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC)
	BIND_GL_FUNCTION(glActiveTexture, PFNGLACTIVETEXTUREPROC)
	BIND_GL_FUNCTION(glUniform1i, PFNGLUNIFORM1IPROC)
	BIND_GL_FUNCTION(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC)
	BIND_GL_FUNCTION(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC)
	BIND_GL_FUNCTION(glUniform3fv, PFNGLUNIFORM3FVPROC)
	BIND_GL_FUNCTION(glUniform4fv, PFNGLUNIFORM4FVPROC)
	
	return true;
}

void OpenGL::GetWorldMatrix(float *matrix)
{
	matrix[0]  = _worldMatrix[0];
	matrix[1]  = _worldMatrix[1];
	matrix[2]  = _worldMatrix[2];
	matrix[3]  = _worldMatrix[3];

	matrix[4]  = _worldMatrix[4];
	matrix[5]  = _worldMatrix[5];
	matrix[6]  = _worldMatrix[6];
	matrix[7]  = _worldMatrix[7];

	matrix[8]  = _worldMatrix[8];
	matrix[9]  = _worldMatrix[9];
	matrix[10] = _worldMatrix[10];
	matrix[11] = _worldMatrix[11];

	matrix[12] = _worldMatrix[12];
	matrix[13] = _worldMatrix[13];
	matrix[14] = _worldMatrix[14];
	matrix[15] = _worldMatrix[15];
}

void OpenGL::GetProjectionMatrix(float *matrix)
{
	matrix[0]  = _projectionMatrix[0];
	matrix[1]  = _projectionMatrix[1];
	matrix[2]  = _projectionMatrix[2];
	matrix[3]  = _projectionMatrix[3];

	matrix[4]  = _projectionMatrix[4];
	matrix[5]  = _projectionMatrix[5];
	matrix[6]  = _projectionMatrix[6];
	matrix[7]  = _projectionMatrix[7];

	matrix[8]  = _projectionMatrix[8];
	matrix[9]  = _projectionMatrix[9];
	matrix[10] = _projectionMatrix[10];
	matrix[11] = _projectionMatrix[11];

	matrix[12] = _projectionMatrix[12];
	matrix[13] = _projectionMatrix[13];
	matrix[14] = _projectionMatrix[14];
	matrix[15] = _projectionMatrix[15];
}

void OpenGL::GetVideoCardInfo(char *cardName)
{
	strcpy_s(cardName, 128, _videoCardDescription);
}

void OpenGL::BuildIdentityMatrix(float *matrix)
{
	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}

void OpenGL::BuildPerspectiveFovLHMatrix(float *matrix, float fieldOfView, float screenAspect, float screenNear, float screenDepth)
{
	matrix[0]  = 1.0f / (screenAspect * static_cast<float>(tan(fieldOfView * 0.5f)));
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f / static_cast<float>(tan(fieldOfView * 0.5f));
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = screenDepth / (screenDepth - screenNear);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-screenNear * screenDepth) / (screenDepth - screenNear);
	matrix[15] = 0.0f;
}

void OpenGL::MatrixRotationY(float *matrix, float angle)
{
	matrix[0]  = cosf(angle);
	matrix[1]  = 0.0f;
	matrix[2]  = -sinf(angle);
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = sinf(angle);
	matrix[9]  = 0.0f;
	matrix[10] = cosf(angle);
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}

void OpenGL::MatrixTranslation(float *matrix, float x, float y, float z)
{
	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0f;
}

void OpenGL::MatrixMultiply(float *result, float *matrix1, float *matrix2)
{
	result[0]  = (matrix1[0] * matrix2[0]) + (matrix1[1] * matrix2[4]) + (matrix1[2] * matrix2[8]) + (matrix1[3] * matrix2[12]);
	result[1]  = (matrix1[0] * matrix2[1]) + (matrix1[1] * matrix2[5]) + (matrix1[2] * matrix2[9]) + (matrix1[3] * matrix2[13]);
	result[2]  = (matrix1[0] * matrix2[2]) + (matrix1[1] * matrix2[6]) + (matrix1[2] * matrix2[10]) + (matrix1[3] * matrix2[14]);
	result[3]  = (matrix1[0] * matrix2[3]) + (matrix1[1] * matrix2[7]) + (matrix1[2] * matrix2[11]) + (matrix1[3] * matrix2[15]);

	result[4]  = (matrix1[4] * matrix2[0]) + (matrix1[5] * matrix2[4]) + (matrix1[6] * matrix2[8]) + (matrix1[7] * matrix2[12]);
	result[5]  = (matrix1[4] * matrix2[1]) + (matrix1[5] * matrix2[5]) + (matrix1[6] * matrix2[9]) + (matrix1[7] * matrix2[13]);
	result[6]  = (matrix1[4] * matrix2[2]) + (matrix1[5] * matrix2[6]) + (matrix1[6] * matrix2[10]) + (matrix1[7] * matrix2[14]);
	result[7]  = (matrix1[4] * matrix2[3]) + (matrix1[5] * matrix2[7]) + (matrix1[6] * matrix2[11]) + (matrix1[7] * matrix2[15]);

	result[8]  = (matrix1[8] * matrix2[0]) + (matrix1[9] * matrix2[4]) + (matrix1[10] * matrix2[8]) + (matrix1[11] * matrix2[12]);
	result[9]  = (matrix1[8] * matrix2[1]) + (matrix1[9] * matrix2[5]) + (matrix1[10] * matrix2[9]) + (matrix1[11] * matrix2[13]);
	result[10] = (matrix1[8] * matrix2[2]) + (matrix1[9] * matrix2[6]) + (matrix1[10] * matrix2[10]) + (matrix1[11] * matrix2[14]);
	result[11] = (matrix1[8] * matrix2[3]) + (matrix1[9] * matrix2[7]) + (matrix1[10] * matrix2[11]) + (matrix1[11] * matrix2[15]);

	result[12] = (matrix1[12] * matrix2[0]) + (matrix1[13] * matrix2[4]) + (matrix1[14] * matrix2[8]) + (matrix1[15] * matrix2[12]);
	result[13] = (matrix1[12] * matrix2[1]) + (matrix1[13] * matrix2[5]) + (matrix1[14] * matrix2[9]) + (matrix1[15] * matrix2[13]);
	result[14] = (matrix1[12] * matrix2[2]) + (matrix1[13] * matrix2[6]) + (matrix1[14] * matrix2[10]) + (matrix1[15] * matrix2[14]);
	result[15] = (matrix1[12] * matrix2[3]) + (matrix1[13] * matrix2[7]) + (matrix1[14] * matrix2[11]) + (matrix1[15] * matrix2[15]);
}
