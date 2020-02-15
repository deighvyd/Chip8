#include "pch.h"

#include "Application.h"

#include "OpenGL.h"
#include "Input.h"
#include "Graphics.h"

Application::Application()
{
	_openGL = nullptr;
	_input = nullptr;
	_graphics = nullptr;
}

Application::Application(const Application& other)
{
}

Application::~Application()
{
}

bool Application::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	_openGL = new OpenGL();
	if (_openGL == nullptr)
	{
		return false;
	}

	if (!InitializeWindows(_openGL, screenWidth, screenHeight))
	{
		MessageBox(_hWnd, L"Could not initialize window", L"Error", MB_OK);
		return false;
	}
	
	if ((_input = new Input()) == nullptr)
	{
		MessageBox(_hWnd, L"Could not create input", L"Error", MB_OK);
		return false;
	}
	_input->Initialize();

	if ((_graphics = new Graphics()) == nullptr)
	{
		MessageBox(_hWnd, L"Could not create graphics", L"Error", MB_OK);
		return false;
	}

	if (!_graphics->Initialize(_openGL, _hWnd))
	{
		MessageBox(_hWnd, L"Could not initialize graphics", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Application::Shutdown()
{
	if (_graphics != nullptr)
	{
		_graphics->Shutdown();
		delete _graphics;
		_graphics = nullptr;
	}

	if (_input != nullptr)
	{
		delete _input;
		_input = nullptr;
	}

	if (_openGL != nullptr)
	{
		_openGL->Shutdown(_hWnd);
		delete _openGL;
		_openGL = nullptr;
	}

	ShutdownWindows();
}

void Application::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool done = false;
	while (!done)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			bool result = RunFrame();
			if(!result)
			{
				done = true;
			}
		}

	}
}

bool Application::RunFrame()
{
	if (_input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	bool result = _graphics->RunFrame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		case WM_KEYDOWN:
		{
			_input->KeyDown((unsigned int)wparam);
			return 0;
		}

		case WM_KEYUP:
		{
			_input->KeyUp((unsigned int)wparam);
			return 0;
		}

		default:
		{
			break;	
		}
	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

bool Application::InitializeWindows(OpenGL* openGL, int& screenWidth, int& screenHeight)
{
	ApplicationHandle = this;
	_hInstance = GetModuleHandle(nullptr);
	_name = L"OpenGL";

	// setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = _hInstance;
	wc.hIcon         = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = _name;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	RegisterClassEx(&wc);

	// create a temporary window for the OpenGL extension setup.
	_hWnd = CreateWindowEx(WS_EX_APPWINDOW, _name, _name, WS_POPUP, 0, 0, 640, 480, nullptr, nullptr, _hInstance, nullptr);
	if (_hWnd == nullptr)
	{
		return false;
	}
	ShowWindow(_hWnd, SW_HIDE);	// don't dhow the window yet

	if (!_openGL->InitializeExtensions(_hWnd))
	{
		MessageBox(_hWnd, L"Could not initialize OpenGL extensions.", L"Error", MB_OK);
		return false;
	}

	// release the temporary window now that the extensions have been initialized.
	DestroyWindow(_hWnd);
	_hWnd = nullptr;

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX, posY;
	if (Graphics::FullScreen)
	{
		// if full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;
	}
	else
	{
		screenWidth  = 800;
		screenHeight = 600;
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// create the window with the screen settings and get the handle to it.
	_hWnd = CreateWindowEx(WS_EX_APPWINDOW, _name, _name, WS_POPUP, posX, posY, screenWidth, screenHeight, nullptr, nullptr, _hInstance, nullptr);
	if (_hWnd == nullptr)
	{
		return false;
	}

	if (!_openGL->InitializeOpenGL(_hWnd, screenWidth, screenHeight, Graphics::ScreenDepth, Graphics::ScreenNear, Graphics::VSyncEnabled))
	{
		MessageBox(_hWnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.", L"Error", MB_OK);
		return false;
	}

	// bring the window up on the screen and set it as main focus.
	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);

	// hide the mouse cursor.
	ShowCursor(false);

	return true;
}

void Application::ShutdownWindows()
{
	ShowCursor(true);

	if (Graphics::FullScreen)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	DestroyWindow(_hWnd);
	_hWnd = nullptr;

	UnregisterClass(_name, _hInstance);
	_hInstance = nullptr;

	ApplicationHandle = nullptr;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		default:
		{
			break;
		}
	}

	return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
}
