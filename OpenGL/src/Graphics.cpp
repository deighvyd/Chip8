#include "pch.h"

#include "Graphics.h"

#include "OpenGL.h"
#include "Camera.h"
#include "Model.h"
#include "ColourShader.h"
#include "TextureShader.h"

Graphics::Graphics()
	: _openGL(nullptr)
	, _camera(nullptr)
	, _model(nullptr)
	, _colourShader(nullptr)
	, _textureShader(nullptr)
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

	if (!_model->Initialize(_openGL, "textures/test.tga", 0, true))
	{
		MessageBox(hWnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	if ((_colourShader = new ColourShader()) == nullptr)
	{
		return false;
	}

	if (!_colourShader->Initialize(_openGL, hWnd))
	{
		MessageBox(hWnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	if ((_textureShader = new TextureShader()) == nullptr)
	{
		return false;
	}

	if (!_textureShader->Initialize(openGL, hWnd))
	{
		MessageBox(hWnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	if (_colourShader)
	{
		_colourShader->Shutdown(_openGL);
		delete _colourShader;
		_colourShader = nullptr;
	}

	if (_textureShader == nullptr)
	{
		_textureShader->Shutdown(_openGL);
		delete _textureShader;
		_textureShader = nullptr;
	}

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
	if (!Render())
	{
		return false;
	}
	
	return true;
}

bool Graphics::Render()
{
	_openGL->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	float worldMatrix[16];
	float viewMatrix[16];
	float projectionMatrix[16];

	// setup the camera matrix
	_camera->Render();

	// get the world, view, and projection matrices from the opengl and camera objects.
	_openGL->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	_openGL->GetProjectionMatrix(projectionMatrix);

	// render the model using the shader

	//Shader* shader = _colourShader;
	Shader* shader = _textureShader;
	
	shader->SetShader(_openGL);
	shader->SetShaderParameters(_openGL, worldMatrix, viewMatrix, projectionMatrix);

	_model->Render(_openGL);

	_openGL->EndScene();

	return true;
}