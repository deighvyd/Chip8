#pragma once

#include "Application.h"

class Chip8;

typedef void* ImTextureID; 

class Designer : public Application
{
public:
	Designer();
	Designer(Designer const&) = delete;
    Designer& operator=(Designer const&) = delete;
	virtual ~Designer();

	virtual bool Initialize(LPCWSTR name) override;

	virtual void OnUpdate(float delta) override;
	virtual void OnDraw() override;
	virtual void OnGui() override;

	virtual void OnKeyDown(unsigned char key) override;
	virtual void OnKeyUp(unsigned char key) override;

private:
	bool UpdateGfxTexture();
	void UploadGfxTexture();

	void DrawPixel(int x, int y, bool filled);

private:
	static int DisplayWidth();
	static int DisplayHeight();

private:
	static constexpr unsigned int DisplayScale = 10;

private:
	Chip8* _chip8 = nullptr;

	bool _paused = true;	// default to paused because Debugger
	unsigned int _step = 0;

	float _clockScale = 1.0f;

	std::string _programFile;
	size_t _programSize = 0;
	unsigned char* _program = nullptr;		// store our own copy of the program so we don't have to pollute the emulator

	ImTextureID _gfxTextureId;
	unsigned char* _gfxTexture = nullptr;

};

