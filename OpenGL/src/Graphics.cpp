#include "pch.h"

#include "Graphics.h"

#include "OpenGL.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
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

	if (!_model->Initialize(_openGL, "models/Cube.txt", "textures/stone.tga", 0, true))
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
	_light = new Light();
	if (_light == nullptr)
	{
		return false;
	}
	_light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	_light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void Graphics::Shutdown()
{
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

bool Graphics::RunFrame()
{
	static float rotation = 0.0f;
	
	// rotate every frame
	rotation += 0.0174532925f * 2.0f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	if (!Render(rotation))
	{
		return false;
	}
	
	return true;
}

bool Graphics::Render(float rotation)
{
	_openGL->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

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

	// rotate the world matrix by the value passed in
	_openGL->MatrixRotationY(worldMatrix, rotation);

	// render the model using the shader

	//Shader* shader = _colourShader;
	//Shader* shader = _textureShader;
	Shader* shader = _lightShader;
	
	shader->SetShader(_openGL);
	shader->SetShaderParameters(_openGL, worldMatrix, viewMatrix, projectionMatrix, 0, lightDirection, diffuseLightColor);

	_model->Render(_openGL);

	_openGL->EndScene();

	return true;
}