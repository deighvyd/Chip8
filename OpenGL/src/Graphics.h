#pragma once

class OpenGL;
class Camera;
class Model;
class Shader;
class Light;

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

	bool Initialize(OpenGL* openGL, HWND hwnd);
	void Shutdown();
	bool RunFrame();

private:
	bool Render(float rotation);

private:
	OpenGL* _openGL;

	Camera* _camera;
	Model* _model;
	Shader* _colourShader;
	Shader* _textureShader;
	Shader* _lightShader;

	Light* _light;

};
