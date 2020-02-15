#pragma once

#include "OpenGL.h"
#include "Input.h"
#include "Graphics.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Application
{
public:
	Application();
	Application(const Application& other);
	~Application();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool RunFrame();
	bool InitializeWindows(OpenGL* openGL, int& screenWidth, int& screenHeight);
	void ShutdownWindows();

private:
	LPCWSTR _name;
	HINSTANCE _hInstance;
	HWND _hWnd;

	OpenGL* _openGL;
	Input* _input;
	Graphics* _graphics;

};

// TODO - Singleton?????
static Application* ApplicationHandle = nullptr;

