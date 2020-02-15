#pragma once

#include "OpenGL.h"

class Model
{
private:
	struct Vertex
	{
		float x, y, z;
		float r, g, b;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(OpenGL *openGL);
	void Shutdown(OpenGL *openGL);
	void Render(OpenGL *openGL);

private:
	bool InitializeBuffers(OpenGL *openGL);
	void ShutdownBuffers(OpenGL *openGL);
	void RenderBuffers(OpenGL *openGL);

private:
	int _vertexCount;
	int _indexCount;
	unsigned int _vertexArrayId;
	unsigned int _vertexBufferId;
	unsigned int _indexBufferId;

};