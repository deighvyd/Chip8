#pragma once

#include "OpenGL.h"

#include "Camera.h"
#include "Model.h"
#include "ColourShader.h"

class Graphics
{
public:
	static constexpr bool FullScreen = false;
	static constexpr bool VSyncEnabled = true;
	static constexpr float ScreenDepth = 1000.0f;
	static constexpr float ScreenNear = 0.1f;

public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Initialize(OpenGL* openGL, HWND hwnd);
	void Shutdown();
	bool RunFrame();

private:
	bool Render();

private:
	OpenGL* _openGL;

	Camera* _camera;
	Model* _model;
	ColourShader* _colourShader;

};
