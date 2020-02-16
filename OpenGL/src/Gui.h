#pragma once

class OpenGL;

struct ImDrawData;

class Gui
{
public:
	Gui();
	Gui(const Gui& other);
	virtual ~Gui();

	bool Initialize(OpenGL* openGL, HWND hWnd);
	void Shutdown(OpenGL* openGL);
	void Render(OpenGL* openGL, HWND hWnd);

private:
	bool SetupRenderState(OpenGL* openGL, ImDrawData* drawData, int fbWidth, int fbHeight, GLuint vertexArrayHandle);

	void NewFrame(HWND hWnd);

	void BuildFonts();

};