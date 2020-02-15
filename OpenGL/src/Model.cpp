#include "pch.h"

#include "Model.h"

#include "OpenGL.h"
#include "Texture.h"

Model::Model()
	: _texture(nullptr)
{
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Initialize(OpenGL *openGL, const char* textureFilename, unsigned int textureUnit, bool wrap)
{
	if (!InitializeBuffers(openGL))
	{
		return false;
	}

	if (!LoadTexture(openGL, textureFilename, textureUnit, wrap))
	{
		return false;
	}

	return true;
}

void Model::Shutdown(OpenGL *openGL)
{
	ReleaseTexture();
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

	vertices[0].tu = 0.0f;  // texture coordinates
	vertices[0].tv = 0.0f;

	// top middle
	vertices[1].x = 0.0f;  // position
	vertices[1].y = 1.0f;
	vertices[1].z = 0.0f;

	vertices[1].tu = 0.5f;  // texture coordinates
	vertices[1].tv = 1.0f;

	// bottom right
	vertices[2].x =  1.0f;  // position
	vertices[2].y = -1.0f;
	vertices[2].z =  0.0f;

	vertices[2].tu = 1.0f;  // texture coordinates
	vertices[2].tv = 0.0f;

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
	openGL->glEnableVertexAttribArray(1);  // texture coordinates

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (unsigned char*)NULL + (3 * sizeof(float)));

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

bool Model::LoadTexture(OpenGL* OpenGL, const char* textureFilename, unsigned int textureUnit, bool wrap)
{	
	if ((_texture = new Texture()) == nullptr)
	{
		return false;
	}

	if (!_texture->Initialize(OpenGL, textureFilename, textureUnit, wrap))
	{
		return false;
	}

	return true;
}

void Model::ReleaseTexture()
{
	if (_texture == nullptr)
	{
		_texture->Shutdown();
		delete _texture;
		_texture = nullptr;
	}
}
