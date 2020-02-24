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

	virtual void OnUpdate() override {}
	virtual void OnDraw() override {}
	virtual void OnGui() override;

protected:
	virtual bool RunFrame() override;

private:
	bool InitGfxTexture();

private:
	static constexpr unsigned int DisplayScale = 10;

private:
	Chip8* _chip8 = nullptr;

	bool _paused = true;	// default to paused because Debugger
	unsigned int _step = 0;

	const char* _programFile;
	size_t _programSize = 0;
	unsigned char* _program = nullptr;		// store our own copy of the program so we don't have to pollute the emulator

	ImTextureID _gfxTextureId;

};

