#include "pch.h"

#include "Graphics.h"

#include "Camera.h"
#include "Gui.h"
#include "Light.h"
#include "Model.h"
#include "OpenGL.h"

#include "ColourShader.h"
#include "LightShader.h"
#include "TextureShader.h"

namespace detail
{
	template<class ShaderType>
	bool InitShader(Shader*& shader, OpenGL* openGL, HWND hWnd)
	{
		if ((shader = new ShaderType()) == nullptr)
		{
			return false;
		}

		if (!shader->Initialize(openGL, hWnd))
		{
			return false;
		}

		return true;
	}

	void DestroyShader(Shader*& shader, OpenGL* openGL)
	{
		if (shader != nullptr)
		{
			shader->Shutdown(openGL);
			delete shader;
			shader = nullptr;
		}
	}
}

Graphics::Graphics()
	: _openGL(nullptr)
	, _camera(nullptr)
	, _model(nullptr)
	, _colourShader(nullptr)
	, _textureShader(nullptr)
	, _lightShader(nullptr)
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

bool Graphics::Initialize(OpenGL* openGL, HWND hWnd)
{
	_openGL = openGL;

	// create the camera, model and shader
	if ((_camera = new Camera()) == nullptr)
	{
		return false;
	}
	_camera->SetPosition(0.0f, 0.0f, -10.0f);

	if ((_model = new Model()) == nullptr)
	{
		return false;
	}

	if (!_model->Initialize(_openGL, "models/cube.txt", "textures/stone.tga", 1, true))
	{
		MessageBox(hWnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	if (!detail::InitShader<ColourShader>(_colourShader, _openGL, hWnd))
	{
		MessageBox(hWnd, L"Could not initialize the colour shader object.", L"Error", MB_OK);
		return false;
	}

	if (!detail::InitShader<TextureShader>(_textureShader, _openGL, hWnd))
	{
		MessageBox(hWnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	if (!detail::InitShader<LightShader>(_lightShader, _openGL, hWnd))
	{
		MessageBox(hWnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

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
	
	if (!_gui->Initialize(_openGL, hWnd))
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

	detail::DestroyShader(_lightShader, _openGL);
	detail::DestroyShader(_textureShader, _openGL);
	detail::DestroyShader(_colourShader, _openGL);
	
	if (_model)
	{
		_model->Shutdown(_openGL);
		delete _model;
		_model = nullptr;
	}

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	_openGL = nullptr;
	return;
}

bool Graphics::RunFrame(HWND hWnd)
{
	static float rotation = 0.0f;
	
	// rotate every frame
	rotation += 0.0174532925f * 1.0f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	if (!Render(hWnd, rotation))
	{
		return false;
	}
	
	return true;
}

bool Graphics::Render(HWND hWnd, float rotation)
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

	// rotate the world matrix by the value passed in
	_openGL->MatrixRotationY(worldMatrix, rotation);

	// render the model using the shader

	//Shader* shader = _colourShader;
	//Shader* shader = _textureShader;
	Shader* shader = _lightShader;
	
	shader->SetShader(_openGL);
	shader->SetShaderParameters(_openGL, worldMatrix, viewMatrix, projectionMatrix, 1, lightDirection, diffuseLightColor, ambientLight);

	_model->Render(_openGL);

	_gui->Render(_openGL, hWnd);

	_openGL->EndScene();

	return true;
}