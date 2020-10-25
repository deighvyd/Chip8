#pragma once

#include "Application.h"

namespace engine
{
	class IEmulator;
}

class eParrot : public Application
{
public:
	eParrot();
	eParrot(eParrot const&) = delete;
    eParrot& operator=(eParrot const&) = delete;
	virtual ~eParrot();

	virtual bool Initialize(LPCWSTR name) override;

	virtual void OnUpdate(float delta) override;
	virtual void OnDraw() override;
	virtual void OnGui() override;

	virtual void OnKeyDown(WPARAM key) override;
	virtual void OnKeyUp(WPARAM key) override;

private:
	void UploadGfxTexture();

private:
	unsigned int GetDisplayWidth() const;
	unsigned int GetDisplayHeight() const;

private:
	static constexpr unsigned int DisplayScale = 10;

private:
	engine::IEmulator* _emulator = nullptr;

	GLuint _gfxTextureId;
	unsigned char* _gfxTexture = nullptr;

};

