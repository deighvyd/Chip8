#pragma once

class Input;
class OpenGL;

struct ImDrawData;

class Gui
{
public:
	Gui();
	Gui(const Gui& other);
	virtual ~Gui();

	bool Initialize(OpenGL* openGL, HWND hWnd, Input* input);
	void Shutdown(OpenGL* openGL);
	void Render(OpenGL* openGL, HWND hWnd, Input* input);

private:
	bool SetupRenderState(OpenGL* openGL, ImDrawData* drawData, int fbWidth, int fbHeight, GLuint vertexArrayHandle);

	void NewFrame(HWND hWnd, Input *input);

	void BuildFonts();

};