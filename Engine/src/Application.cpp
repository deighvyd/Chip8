#include "pch.h"

#include "Application.h"

#include "Graphics.h"
#include "Input.h"
#include "Log.h"
#include "OpenGL.h"

namespace detail
{
	static Application* Instance = nullptr;

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

		return Application::Instance()->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}

Application* Application::Instance()
{
	return detail::Instance;
}

Application::Application(int width, int height)
	: _width(width)
	, _height(height)
	, _hasFocus(false)
	, _openGL(nullptr)
	, _input( nullptr)
	, _graphics(nullptr)
{
	assert(detail::Instance == nullptr);
	detail::Instance = this;
}

Application::~Application()
{
	detail::Instance = nullptr;
}

bool Application::Initialize(LPCWSTR name)
{
	_name = name;
	int screenWidth = _width;
	int screenHeight = _height;

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

	if (!_graphics->Initialize(_openGL, _hWnd, _input))
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

		switch (msg.message)
		{
			case WM_QUIT:
			{
				done = true;
				break;
			}

			/*case WM_SIZE:
			{
				glViewport(0, 0, (GLsizei)LOWORD(msg.lParam), (GLsizei)HIWORD(msg.lParam)); 
				break;
			}*/

			default:
			{
				break;
			}
		}
		
		if (!done)
		{
			if (!RunFrame())
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

	// TODO - can be more accurate that this
	int currTime = timeGetTime();
    float deltaTime = _time > 0 ? (static_cast<float>(currTime - _time) * 0.001f) : (1.0f / 60.0f);;
    _time = currTime;

	OnUpdate(deltaTime);
	OnDraw();

	bool result = _graphics->RunFrame(_hWnd, _input, deltaTime);
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Application::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SETFOCUS:
		{
			_hasFocus = true;
			//logging::Info("Gained focus");
			break;
		}
  
		case WM_KILLFOCUS:
		{
			_hasFocus = false;
			//logging::Info("Lost focus");
			break;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			_input->KeyDown((unsigned int)wParam);
			return 0;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			_input->KeyUp((unsigned int)wParam);
			return 0;
		}

		case WM_CHAR:
		{
			_input->OnCharacterPressed((unsigned int)wParam);
			break;
		}

		case WM_LBUTTONUP:
		{
			_input->OnMouseUp(0);
			break;
		}

		case WM_LBUTTONDOWN:
		{
			_input->OnMouseDown(0);
			break;
		}
			
		case WM_MBUTTONUP:
		{
			_input->OnMouseUp(1);
			break;
		}

		case WM_MBUTTONDOWN:
		{
			_input->OnMouseDown(1);
			break;
		}

		case WM_RBUTTONUP:
		{
			_input->OnMouseUp(2);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			_input->OnMouseDown(2);
			break;
		}

		case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam);
			_input->OnMouseMove(xPos, yPos);
			break;
		}

		default:
		{
			break;	
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool Application::InitializeWindows(OpenGL* openGL, int& screenWidth, int& screenHeight)
{
	_hInstance = GetModuleHandle(nullptr);

	// setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = detail::WndProc;
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

	// create a temporary window for the OpenGL extension setup
	_hWnd = CreateWindowEx(WS_EX_APPWINDOW, _name, _name, WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, nullptr, nullptr, _hInstance, nullptr);
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
		screenWidth  = _width;
		screenHeight = _height;
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	RECT wRect;			
	wRect.left =	(long)0;
	wRect.right =	(long)screenWidth;
	wRect.top =		(long)0;
	wRect.bottom =	(long)screenHeight;

	DWORD wStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	if (!AdjustWindowRectEx(&wRect, WS_OVERLAPPEDWINDOW, false, wStyle))
	{
		MessageBox(_hWnd, L"Could adjust the window rectangle", L"Error", MB_OK);
		return false;
	}

	// create the window with the screen settings and get the handle to it
	_hWnd = CreateWindowEx(wStyle, _name, _name, WS_OVERLAPPEDWINDOW  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, posX, posY, wRect.right - wRect.left, wRect.bottom - wRect.top, nullptr, nullptr, _hInstance, nullptr);
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

	return true;
}

void Application::ShutdownWindows()
{
	if (Graphics::FullScreen)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	DestroyWindow(_hWnd);
	_hWnd = nullptr;

	UnregisterClass(_name, _hInstance);
	_hInstance = nullptr;

	return;
}

