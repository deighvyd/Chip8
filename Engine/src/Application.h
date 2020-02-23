#pragma once

class OpenGL;
class Input;
class Graphics;

class Application
{
public:
	static Application* Instance();

public:
	bool Initialize(LPCWSTR name);
	void Shutdown();
	void Run();

	bool HasFocus() const { return _hasFocus; }

	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;
	virtual void OnGui() = 0;

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual bool RunFrame();

private:
	bool InitializeWindows(OpenGL* openGL, int& screenWidth, int& screenHeight);
	void ShutdownWindows();

protected:
	Application(int width, int height);
	Application(Application const&) = delete;
    Application& operator=(Application const&) = delete;
	virtual ~Application();

protected:
	LPCWSTR _name;
	HINSTANCE _hInstance;
	HWND _hWnd;

	int _width;
	int _height;

	bool _hasFocus;

	OpenGL* _openGL;
	Input* _input;
	Graphics* _graphics;

};

