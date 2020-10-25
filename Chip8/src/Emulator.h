#pragma once

#include "IEmulator.h"

#include "Chip8.h"

namespace chip8
{
	class Emulator : public engine::IEmulator
    {
	public:
		Emulator();
		virtual ~Emulator();

		unsigned int GetScreenWidth() const override { return Chip8::ScreenWidth; }
		unsigned int GetScreenHeight() const override { return Chip8::ScreenHeight; }

        void Update(float delta) override;
		bool Draw(unsigned int scale, unsigned char* gfxTexture) override;	// TODO - this probably should not have an unchecked pointer passed in

		void OnKeyUp(unsigned int param) override;
		void OnKeyDown(unsigned int param) override;

		void OnGui(unsigned int width, unsigned int height);

	private:
		static void DrawPixel(unsigned int x, unsigned int y, unsigned int scale, bool filled, unsigned char* gfxTexture);

	private:
		Chip8 _chip8;

		bool _paused = true;	// default to paused because Debugger
		unsigned int _step = 0;

		float _clockScale = 1.5f;

		std::string _programFile;
		size_t _programSize = 0;
		unsigned char* _program = nullptr;		// store our own copy of the program so we don't have to pollute the emulator

    };
}
