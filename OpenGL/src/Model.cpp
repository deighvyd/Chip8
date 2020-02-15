#include "pch.h"

#include "Model.h"

Model::Model()
{
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Initialize(OpenGL *openGL)
{
	if (!InitializeBuffers(openGL))
	{
		return false;
	}

	return true;
}

void Model::Shutdown(OpenGL *openGL)
{
	ShutdownBuffers(openGL);
}

void Model::Render(OpenGL *openGL)
{
	RenderBuffers(openGL);
}

bool Model::InitializeBuffers(OpenGL *openGL)
{
	// build a triangle
	_vertexCount = 3;
	_indexCount = 3;

	Vertex* vertices = new Vertex[_vertexCount];
	if (vertices == nullptr)
	{
		return false;
	}

	unsigned int *indices = new unsigned int[_indexCount];
	if (indices == nullptr)
	{
		return false;
	}

	// bottom left
	vertices[0].x = -1.0f;  // position
	vertices[0].y = -1.0f;
	vertices[0].z =  0.0f;

	vertices[0].r = 0.0f;  // color
	vertices[0].g = 1.0f;
	vertices[0].b = 0.0f;

	// top middle
	vertices[1].x = 0.0f;  // position
	vertices[1].y = 1.0f;
	vertices[1].z = 0.0f;

	vertices[1].r = 0.0f;  // color
	vertices[1].g = 1.0f;
	vertices[1].b = 0.0f;

	// bottom right
	vertices[2].x =  1.0f;  // position
	vertices[2].y = -1.0f;
	vertices[2].z =  0.0f;

	vertices[2].r = 0.0f;  // color
	vertices[2].g = 1.0f;
	vertices[2].b = 0.0f;

	// load the index array with data
	indices[0] = 0;  // bottom left
	indices[1] = 1;  // top middle
	indices[2] = 2;  // bottom right

	// create and bind vertex buffer
	openGL->glGenVertexArrays(1, &_vertexArrayId);
	openGL->glBindVertexArray(_vertexArrayId);
	openGL->glGenBuffers(1, &_vertexBufferId);

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glBufferData(GL_ARRAY_BUFFER, _vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	openGL->glEnableVertexAttribArray(0);  // vertex position
	openGL->glEnableVertexAttribArray(1);  // vertex color

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (unsigned char*)NULL + (3 * sizeof(float)));

	// create and bind index buffer
	openGL->glGenBuffers(1, &_indexBufferId);

	openGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
	openGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount* sizeof(unsigned int), indices, GL_STATIC_DRAW);

	delete [] vertices;
	vertices = nullptr;

	delete [] indices;
	indices = nullptr;

	return true;
}

void Model::ShutdownBuffers(OpenGL *openGL)
{
	openGL->glDisableVertexAttribArray(0);
	openGL->glDisableVertexAttribArray(1);
	
	openGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
	openGL->glDeleteBuffers(1, &_vertexBufferId);

	openGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	openGL->glDeleteBuffers(1, &_indexBufferId);

	openGL->glBindVertexArray(0);
	openGL->glDeleteVertexArrays(1, &_vertexArrayId);
}

void Model::RenderBuffers(OpenGL *openGL)
{
	openGL->glBindVertexArray(_vertexArrayId);
	glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
}
