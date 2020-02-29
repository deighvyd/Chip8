#include "pch.h"

#include "Graphics.h"

#include "Application.h"
#include "Camera.h"
#include "Gui.h"
#include "Light.h"
#include "OpenGL.h"

Graphics::Graphics()
	: _openGL(nullptr)
	, _camera(nullptr)
	, _light(nullptr)
	, _gui(nullptr)
{
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(OpenGL* openGL, HWND hWnd, Input* input)
{
	_openGL = openGL;

	// create the camera, model and shader
	if ((_camera = new Camera()) == nullptr)
	{
		return false;
	}
	_camera->SetPosition(0.0f, 0.0f, -10.0f);

	// create a light object
	if ((_light = new Light()) == nullptr)
	{
		return false;
	}
	_light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	_light->SetDirection(0.0f, 0.0f, 1.0f);
	_light->SetDirection(1.0f, 0.0f, 0.0f);
	_light->SetAmbientLight(0.15f, 0.15f, 0.15f, 1.0f);

	// create and initialize the Gui
	if ((_gui = new Gui()) == nullptr)
	{
		MessageBox(hWnd, L"Could not create the Gui.", L"Error", MB_OK);
		return false;
	}
	
	if (!_gui->Initialize(_openGL, hWnd, input))
	{
		MessageBox(hWnd, L"Could not initliaze the Gui.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	if (_gui != nullptr)
	{
		_gui->Shutdown(_openGL);
		delete _gui;
		_gui = nullptr;
	}

	if (_light != nullptr)
	{
		delete _light;
		_light = nullptr;
	}

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	_openGL = nullptr;
	return;
}

bool Graphics::RunFrame(HWND hWnd, Input* input, float delta)
{
	static float rotation = 0.0f;
	
	// rotate every frame
	rotation += 0.0174532925f * 1.0f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	if (!Render(hWnd, input, rotation, delta))
	{
		return false;
	}
	
	return true;
}

bool Graphics::Render(HWND hWnd, Input* input, float rotation, float delta)
{
	_openGL->BeginScene(0.45f, 0.55f, 0.60f, 1.00f);

	// setup the camera matrix
	_camera->Render();

	// get the world, view, and projection matrices from the opengl and camera objects.
	float worldMatrix[16];
	_openGL->GetWorldMatrix(worldMatrix);

	float viewMatrix[16];
	_camera->GetViewMatrix(viewMatrix);
	
	float projectionMatrix[16];
	_openGL->GetProjectionMatrix(projectionMatrix);

	float lightDirection[3];
	_light->GetDirection(lightDirection);

	float diffuseLightColor[4];
	_light->GetDiffuseColour(diffuseLightColor);

	float ambientLight[4];
	_light->GetAmbientLight(ambientLight);

	// TODO - this is shit I dont want to call out to the application instance here
	_gui->BeginRender(_openGL, hWnd, input, delta);
	Application::Instance()->OnGui();
	_gui->EndRender(_openGL, hWnd, input);

	_openGL->EndScene();

	return true;
}