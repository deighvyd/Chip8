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
	~Model();

	bool Initialize(OpenGL *openGL, const char* textureFilename, unsigned int textureUnit, bool wrap);
	void Shutdown(OpenGL *openGL);
	void Render(OpenGL *openGL);

private:
	bool InitializeBuffers(OpenGL *openGL);
	void ShutdownBuffers(OpenGL *openGL);
	void RenderBuffers(OpenGL *openGL);

	bool LoadTexture(OpenGL* openGL, const char*, unsigned int, bool);
	void ReleaseTexture();

private:
	int _vertexCount;
	int _indexCount;
	unsigned int _vertexArrayId;
	unsigned int _vertexBufferId;
	unsigned int _indexBufferId;

	Texture* _texture;

};