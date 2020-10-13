#include "pch.h"

#include "Shader.h"

#include "OpenGL.h"

using namespace std;

Shader::Shader()
{
}

Shader::Shader(const Shader&)
{
}

Shader::~Shader()
{
}

void Shader::Shutdown(OpenGL* openGL)
{
	ShutdownShader(openGL);
}

void Shader::SetShader(OpenGL* openGL)
{
	openGL->glUseProgram(_shaderProgram);
}

bool Shader::SetShaderParameters(OpenGL* openGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, float* lightDirection, float* diffuseLightColour, float* ambientLight)
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

void Shader::BindAttributes(OpenGL* openGL) const
{
	openGL->glBindAttribLocation(_shaderProgram, 0, "inputPosition");
	openGL->glBindAttribLocation(_shaderProgram, 1, "inputColour");
	openGL->glBindAttribLocation(_shaderProgram, 2, "inputNormal");
}

bool Shader::LoadShader(const char* vsFilename, const char* fsFilename, OpenGL* openGL, HWND hWnd)
{
	const char *vsSource = LoadShaderSourceFile(vsFilename);
	if (vsSource == nullptr)
	{
		return false;
	}

	const char *fsSource = LoadShaderSourceFile(fsFilename);
	if (fsSource == nullptr)
	{
		return false;
	}

	bool result = CompileShader(vsSource, fsSource, openGL, hWnd);

	delete [] vsSource;
	delete [] fsSource;
	vsSource = fsSource = nullptr;

	return result;
}

bool Shader::CompileShader(const char* vsSource, const char* fsSource, OpenGL* openGL, HWND hWnd)
{
	// load and copile the shaders
	_vertexShader = openGL->glCreateShader(GL_VERTEX_SHADER);
	_fragmentShader = openGL->glCreateShader(GL_FRAGMENT_SHADER);

	openGL->glShaderSource(_vertexShader, 1, &vsSource, nullptr);
	openGL->glShaderSource(_fragmentShader, 1, &fsSource, nullptr);

	openGL->glCompileShader(_vertexShader);
	openGL->glCompileShader(_fragmentShader);

	// check for errors
	int status;
	openGL->glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		OutputShaderErrorMessage(openGL, hWnd, _vertexShader, vsSource);
		return false;
	}

	openGL->glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		OutputShaderErrorMessage(openGL, hWnd, _fragmentShader, vsSource);
		return false;
	}

	// create the shader program
	_shaderProgram = openGL->glCreateProgram();

	openGL->glAttachShader(_shaderProgram, _vertexShader);
	openGL->glAttachShader(_shaderProgram, _fragmentShader);

	BindAttributes(openGL);

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

char* Shader::LoadShaderSourceFile(const char* filename)
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

void Shader::OutputShaderErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int shaderId, const char* shaderFilename)
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
	size_t convertedChars;
	unsigned int error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
	if (error != 0)
	{
		return;
	}
	MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", newString, MB_OK);
}

void Shader::OutputLinkerErrorMessage(OpenGL* openGL, HWND hWnd, unsigned int programId)
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

void Shader::ShutdownShader(OpenGL* openGL)
{
	openGL->glDetachShader(_shaderProgram, _vertexShader);
	openGL->glDetachShader(_shaderProgram, _fragmentShader);

	openGL->glDeleteShader(_vertexShader);
	openGL->glDeleteShader(_fragmentShader);

	openGL->glDeleteProgram(_shaderProgram);
}

void Shader::DestroyShader(Shader*& shader, OpenGL* openGL)
{
	if (shader != nullptr)
	{
		shader->Shutdown(openGL);
		delete shader;
		shader = nullptr;
	}
}