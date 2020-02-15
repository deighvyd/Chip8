#pragma once

class OpenGL;
class Texture;

class Model
{
private:
	struct Vertex
	{
		float x, y, z;		// position
		float tu, tv;		// tex coords
		float nx, ny, nz;	// normal
	};

public:
	Model();
	Model(const Model&);
	virtual ~Model();

	bool Initialize(OpenGL *openGL, const char* modelFilename, const char* textureFilename, unsigned int textureUnit, bool wrap);
	void Shutdown(OpenGL *openGL);
	void Render(OpenGL *openGL);

private:
	bool InitializeBuffers(OpenGL *openGL);
	void ShutdownBuffers(OpenGL *openGL);
	void RenderBuffers(OpenGL *openGL);

	bool LoadModel(const char* filename);
	void ReleaseModel();

	bool LoadTexture(OpenGL* openGL, const char* textureFilename, unsigned int textureUnit, bool wrap);
	void ReleaseTexture();

private:
	int _vertexCount;
	int _indexCount;
	Vertex* _vertices;
	Texture* _texture;

	unsigned int _vertexArrayId;
	unsigned int _vertexBufferId;
	unsigned int _indexBufferId;

};