#pragma once

class OpenGL;
class Input;
class Graphics;

class Application
{
public:
	static Application* Instance();

public:
	virtual bool Initialize(LPCWSTR name);
	void Shutdown();
	void Run();

	bool HasFocus() const { return _hasFocus; }

	virtual void OnUpdate(float delta) {}
	virtual void OnDraw() {}
	virtual void OnGui() {}

	virtual void OnKeyDown(WPARAM key) {}
	virtual void OnKeyUp(WPARAM key) {}

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool InitializeWindows(OpenGL* openGL, int& screenWidth, int& screenHeight);
	void ShutdownWindows();

	bool RunFrame();

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

	// track time using QPC
	LARGE_INTEGER _time;
	LARGE_INTEGER _frequency;

	OpenGL* _openGL;
	Input* _input;
	Graphics* _graphics;

};

