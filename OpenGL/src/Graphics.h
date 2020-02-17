#pragma once

class Camera;
class Gui;
class Input;
class Light;
class Model;
class OpenGL;
class Shader;

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
	virtual ~Graphics();

	bool Initialize(OpenGL* openGL, HWND hwnd, Input* input);
	void Shutdown();
	bool RunFrame(HWND hWnd, Input* input);

private:
	bool Render(HWND hWnd, Input* input, float rotation);

private:
	OpenGL* _openGL;

	Camera* _camera;
	Model* _model;
	Shader* _colourShader;
	Shader* _textureShader;
	Shader* _lightShader;

	Light* _light;

	Gui* _gui;

};
