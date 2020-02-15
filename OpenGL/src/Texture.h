#pragma once

class OpenGL;

class Texture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(OpenGL* openGL, const char* filename, unsigned int textureUnit, bool wrap);
	void Shutdown();

private:
	bool LoadTarga(OpenGL* openGL, const char* filename, unsigned int textureUnit, bool wrap);

private:
	bool _loaded;
	unsigned int _textureID;

};