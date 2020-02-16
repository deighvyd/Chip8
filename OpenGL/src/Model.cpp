#include "pch.h"

#include "Model.h"

#include "OpenGL.h"
#include "Texture.h"

using namespace std;

Model::Model()
	: _texture(nullptr)
	, _vertices(nullptr)
{
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Initialize(OpenGL *openGL, const char* modelFilename, const char* textureFilename, unsigned int textureUnit, bool wrap)
{
	if (!LoadModel(modelFilename))
	{
		return false;
	}

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
	ReleaseModel();
}

void Model::Render(OpenGL *openGL)
{
	// setup the render state
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_SCISSOR_TEST);

	RenderBuffers(openGL);
}

bool Model::InitializeBuffers(OpenGL *openGL)
{
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

	for (int i = 0 ; i < _vertexCount ; ++i)
	{
		vertices[i].x  = _vertices[i].x;
		vertices[i].y  = _vertices[i].y;
		vertices[i].z  = _vertices[i].z;
		vertices[i].tu = _vertices[i].tu;
		vertices[i].tv = 1.0f - _vertices[i].tv;
		vertices[i].nx = _vertices[i].nx;
		vertices[i].ny = _vertices[i].ny;
		vertices[i].nz = _vertices[i].nz;

		indices[i] = i;
	}

	// create and bind vertex buffer
	openGL->glGenVertexArrays(1, &_vertexArrayId);
	openGL->glBindVertexArray(_vertexArrayId);
	openGL->glGenBuffers(1, &_vertexBufferId);

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glBufferData(GL_ARRAY_BUFFER, _vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	openGL->glEnableVertexAttribArray(0);  // vertex position
	openGL->glEnableVertexAttribArray(1);  // texture coordinate
	openGL->glEnableVertexAttribArray(2);  // mormal

	// set the formats (striding)
	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (unsigned char*)NULL + (3 * sizeof(float)));

	openGL->glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	openGL->glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (unsigned char*)NULL + (5 * sizeof(float)));

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

bool Model::LoadModel(const char* filename)
{
	// open the model file
	ifstream fin(filename);
	if (fin.fail())
	{
		return false;
	}

	// read the vertex count
	char input;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> _vertexCount;
	_indexCount = _vertexCount;

	// create the model
	_vertices = new Vertex[_vertexCount];
	if (_vertices == nullptr)
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	for (int i = 0 ; i < _vertexCount ; ++i)
	{
		fin >> _vertices[i].x >> _vertices[i].y >> _vertices[i].z;
		fin >> _vertices[i].tu >> _vertices[i].tv;
		fin >> _vertices[i].nx >> _vertices[i].ny >> _vertices[i].nz;
	}
	fin.close();

	return true;
}

void Model::ReleaseModel()
{
	if (_vertices)
	{
		delete [] _vertices;
		_vertices = nullptr;
	}
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
