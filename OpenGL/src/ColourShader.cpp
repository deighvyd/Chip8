#include "pch.h"

#include "ColourShader.h"

using namespace std;

ColourShader::ColourShader()
{
}

ColourShader::ColourShader(const ColourShader& other)
{
}

ColourShader::~ColourShader()
{
}

bool ColourShader::Initialize(OpenGL* openGL, HWND hWnd)
{
	if (!InitializeShader("shaders/colour.vs", "shaders/colour.ps", openGL, hWnd))
	{
		return false;
	}

	return true;
}

void ColourShader::Shutdown(OpenGL* openGL)
{
	ShutdownShader(openGL);
}

void ColourShader::SetShader(OpenGL* openGL)
{
	openGL->glUseProgram(_shaderProgram);
}

bool ColourShader::InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* openGL, HWND hWnd)
{
	const char *vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
	if (vertexShaderBuffer == nullptr)
	{
		return false;
	}

	const char *fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
	if (fragmentShaderBuffer == nullptr)
	{
		return false;
	}

	// load and copile the shaders
	_vertexShader = openGL->glCreateShader(GL_VERTEX_SHADER);
	_fragmentShader = openGL->glCreateShader(GL_FRAGMENT_SHADER);

	openGL->glShaderSource(_vertexShader, 1, &vertexShaderBuffer, nullptr);
	openGL->glShaderSource(_fragmentShader, 1, &fragmentShaderBuffer, nullptr);

	delete [] vertexShaderBuffer;
	delete [] fragmentShaderBuffer;
	vertexShaderBuffer = fragmentShaderBuffer = nullptr;

	openGL->glCompileShader(_vertexShader);
	openGL->glCompileShader(_fragmentShader);

	// check for errors
	int status;
	openGL->glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		OutputShaderErrorMessage(openGL, hWnd, _vertexShader, vsFilename);
		return false;
	}

	openGL->glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		OutputShaderErrorMessage(openGL, hWnd, _fragmentShader, fsFilename);
		return false;
	}

	// create the shader program
	_shaderProgram = openGL->glCreateProgram();

	openGL->glAttachShader(_shaderProgram, _vertexShader);
	openGL->glAttachShader(_shaderProgram, _fragmentShader);

	openGL->glBindAttribLocation(_shaderProgram, 0, "inputPosition");
	openGL->glBindAttribLocation(_shaderProgram, 1, "inputColor");

	openGL->glLinkProgram(_shaderProgram);

	// check for errors
	openGL->glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (status != 1)
	{
		OutputLinkerErrorMessage(openGL, hWnd, _shaderProgram);
		return false;
	}

	return true;
}

char *ColourShader::LoadShaderSourceFile(const char* filename)
{
	// open the sorce file
	ifstream fin(filename);
	if (fin.fail())
	{
		return nullptr;
	}

	// read the file size
	char input;
	int fileSize = 0;
	
	fin.get(input);
	while(!fin.eof())
	{
		fileSize++;
		fin.get(input);
	}
	fin.close();

	// read the file into a buffer
	char* buffer = new char[fileSize+1];
	if (buffer == nullptr)
	{
		return nullptr;
	}

	fin.open(filename);
	fin.read(buffer, fileSize);
	fin.close();
	buffer[fileSize] = '\0';

	return buffer;
}

void ColourShader::OutputShaderErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int shaderId, const char* shaderFilename)
{
	// read the shader log message 
	int logSize;
	openGL->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
	logSize++;	// null terinator

	char* infoLog = new char[logSize];
	if (infoLog == nullptr)
	{
		return;
	}
	openGL->glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);
	
	// write the log to a file
	ofstream fout("shader-error.txt");
	for (int i = 0 ; i < logSize ; ++i)
	{
		fout << infoLog[i];
	}

	fout.close();

	// show in a message box
	wchar_t newString[128];
	unsigned int convertedChars;
	unsigned int error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
	if (error != 0)
	{
		return;
	}
	MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", newString, MB_OK);
}

void ColourShader::OutputLinkerErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int programId)
{
	// read the log message
	int logSize;
	openGL->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);
	logSize++;	// null terminator

	// Create a char buffer to hold the info log.
	char *infoLog = new char[logSize];
	if (infoLog == nullptr)
	{
		return;
	}
	openGL->glGetProgramInfoLog(programId, logSize, nullptr, infoLog);

	// write the message to a file
	ofstream fout("linker-error.txt");
	for (int i = 0 ; i < logSize ; ++i)
	{
		fout << infoLog[i];
	}
	fout.close();

	// show the error
	MessageBox(hWnd, L"Error compiling linker.  Check linker-error.txt for message.", L"Linker Error", MB_OK);
}

void ColourShader::ShutdownShader(OpenGL* openGL)
{
	openGL->glDetachShader(_shaderProgram, _vertexShader);
	openGL->glDetachShader(_shaderProgram, _fragmentShader);

	openGL->glDeleteShader(_vertexShader);
	openGL->glDeleteShader(_fragmentShader);

	openGL->glDeleteProgram(_shaderProgram);
}

bool ColourShader::SetShaderParameters(OpenGL* openGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix)
{
	unsigned int location = openGL->glGetUniformLocation(_shaderProgram, "worldMatrix");
	if (location == -1)
	{
		return false;
	}
	openGL->glUniformMatrix4fv(location, 1, false, worldMatrix);

	location = openGL->glGetUniformLocation(_shaderProgram, "viewMatrix");
	if (location == -1)
	{
		return false;
	}
	openGL->glUniformMatrix4fv(location, 1, false, viewMatrix);

	location = openGL->glGetUniformLocation(_shaderProgram, "projectionMatrix");
	if (location == -1)
	{
		return false;
	}
	openGL->glUniformMatrix4fv(location, 1, false, projectionMatrix);

	return true;
}
