#include "pch.h"

#include "Texture.h"

#include "OpenGL.h"

Texture::Texture()
{
	_loaded = false;
}

Texture::Texture(const Texture& other)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(OpenGL* openGL, const char* filename, unsigned int textureUnit, bool wrap)
{
	if (!LoadTarga(openGL, filename, textureUnit, wrap))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	if (_loaded)
	{
		glDeleteTextures(1, &_textureID);
		_loaded = false;
	}
}

bool Texture::LoadTarga(OpenGL* openGL, const char* filename, unsigned int textureUnit, bool wrap)
{
	/*int error, width, height, bpp, imageSize;
	FILE* file;
	unsigned int count;
	
	unsigned char* targaImage;*/

	// open the file and read in 
	FILE* file;
	int error = fopen_s(&file, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	TargaHeader targaFileHeader;
	unsigned int count = fread(&targaFileHeader, sizeof(TargaHeader), 1, file);
	if (count != 1)
	{
		return false;
	}

	int width = (int)targaFileHeader.width;
	int height = (int)targaFileHeader.height;
	int bpp = (int)targaFileHeader.bpp;
	if (bpp != 32)	// check that it is 32 bit and not 24 bit.
	{
		return false;
	}

	// allocate a buffer and read the image
	int imageSize = width * height * 4;
	unsigned char *targaImage = new unsigned char[imageSize];
	if (targaImage == nullptr)
	{
		return false;
	}

	count = fread(targaImage, 1, imageSize, file);
	if (count != imageSize)
	{
		return false;
	}
	
	error = fclose(file);
	if (error != 0)
	{
		return false;
	}

	// setup a texture unit for the texture
	openGL->glActiveTexture(GL_TEXTURE0 + textureUnit);
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, targaImage);

	// det the texture colour to either wrap around or clamp to the edge.
	if (wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// set the texture filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// generate mipmaps for the texture.
	openGL->glGenerateMipmap(GL_TEXTURE_2D);

	delete [] targaImage;
	targaImage = nullptr;

	_loaded = true;		// finished loading

	return true;
}
