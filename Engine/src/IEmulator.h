#pragma once

namespace engine
{
	class IEmulator
	{
	public:
		virtual unsigned int GetScreenWidth() const = 0;
		virtual unsigned int GetScreenHeight() const = 0;

		virtual void Update(float delta) = 0;
		virtual bool Draw(unsigned int scale, unsigned char* gfxTexture) = 0;	// TODO - this probably should not have an unchecked pointer passed in

		virtual void OnKeyUp(unsigned int key) = 0;
		virtual void OnKeyDown(unsigned int key) = 0;

		virtual void OnGui(unsigned int width, unsigned int height) = 0;

	};
}