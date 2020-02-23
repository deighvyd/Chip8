#pragma once

// Chip-8 emulator
// https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
class Chip8
{
public:
	// specifications
	// TODO - move this its own header files?

	static constexpr int TotalMemoryBytes = 0x1000;
	static constexpr int NumRegisers = 16;

	// Memory maps
	//0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
	//0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
	//0x200 - 0xFFF - Program ROM and work RAM
	static constexpr short ProgramStart = 0x200;

	static constexpr int ScreenWidth = 64;
	static constexpr int ScreenHeight = 32;

	static constexpr int StackSize = 16;

	static constexpr int NumKeys = 16;

	static constexpr int CyclesPerSecond = 60;

	static constexpr unsigned int NumFontChars = 80;
	static constexpr unsigned char Font[NumFontChars] = 
	{ 
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

public:
	Chip8();
	Chip8(const char* filename);

	void Initialize();
	bool LoadProgram(const char* filename);
	void EmulateCycle();

	// meta accessors
	unsigned short PC() const { return _pc; }

public:
	static size_t ReadProgram(const char* filename, unsigned char* buffer, size_t bufferSize);

private:
	unsigned char _memory[TotalMemoryBytes];	// 4K of memory

	unsigned char _v[NumRegisers];
	unsigned short _i;
	unsigned short _pc;

	unsigned char _gfx[ScreenWidth * ScreenHeight];

	unsigned char _delayTimer;
	unsigned char _soundTimer;

	//unsigned short _opcode;
	unsigned short _stack[StackSize];
	unsigned short _sp;

	unsigned char _keys[NumKeys];

};