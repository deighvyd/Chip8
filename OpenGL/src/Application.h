#pragma once

class OpenGL;
class Input;
class Graphics;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Application
{
public:
	static Application& GetInstance();

public:
	virtual ~Application();

	bool Initialize();
	void Shutdown();
	void Run();

	bool HasFocus() const { return _hasFocus; }

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool RunFrame();
	bool InitializeWindows(OpenGL* openGL, int& screenWidth, int& screenHeight);
	void ShutdownWindows();

private:
	Application();
	Application(const Application& other);

private:
	static Application* _Instance;

private:
	LPCWSTR _name;
	HINSTANCE _hInstance;
	HWND _hWnd;

	bool _hasFocus;

	OpenGL* _openGL;
	Input* _input;
	Graphics* _graphics;

};

