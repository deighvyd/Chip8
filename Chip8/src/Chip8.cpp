#include "pch.h"

#include "Chip8.h"

#include "Log.h"

using namespace logging;

Chip8::Chip8()
{
	Initialize();
}

Chip8::Chip8(const char* filename)
{
	Initialize();
	LoadProgram(filename);
}

void Chip8::Initialize()
{
	memset(_memory, 0, TotalMemoryBytes);
	
	memset(_v, 0, NumRegisers);
	_i = 0;
	_pc = ProgramStart;

	memset(_gfx, 0, ScreenWidth * ScreenHeight);

	_delayTimer = 0;
	_soundTimer = 0;

	//_opcode = 0;
	memset(_stack, 0, StackSize);
	_sp = 0;

	memset(_keys, 0, NumKeys);

	// Load fontset
	memcpy(_memory + FontStart, Font, NumFontChars);

	_draw = false;

	// TODO - this is not a great place for this
	// TODO - make a better rand generator
	time_t t;
	srand((unsigned) time(&t));
}

unsigned char Chip8::Register(unsigned int reg) const
{
	if (reg >= NumRegisers)
	{
		return 0;
	}

	return _v[reg];
}

unsigned char Chip8::Memory(unsigned int loc) const
{
	if (loc >= TotalMemoryBytes)
	{
		return 0;
	}

	return _memory[loc];
}

unsigned char Chip8::Pixel(int x, int y) const
{
	if (x < 0 || x >= ScreenWidth || y < 0 || y > ScreenHeight)
	{
		Info("Error: pixel out of range (%d, %d)", x, y);
		return 0;
	}

	return _gfx[x + (ScreenWidth * y)];
}

bool Chip8::Key(unsigned int key) const
{
	if (key >= NumKeys)
	{
		Info("Error: unknown key %u", key);
		return false;
	}

	return _keys[key];
}

void Chip8::KeyDown(unsigned int key)
{
	if (key >= NumKeys)
	{
		Info("Error: unknown key %u", key);
		return;
	}

	_keys[key] = true;
}

void Chip8::KeyUp(unsigned int key)
{
	if (key >= NumKeys)
	{
		Info("Error: unknown key %u", key);
		return;
	}

	_keys[key] = false;
}

bool Chip8::LoadProgram(const char* filename) 
{
	size_t size = ReadProgram(filename, &(_memory[ProgramStart]), TotalMemoryBytes - ProgramStart);
	if (size == 0)
	{
		Info("Error: could not load program %s\n", filename);
		return false;
	}
	
	return true;
}

size_t Chip8::ReadProgram(const char* filename, unsigned char* buffer, size_t bufferSize)
{
	assert(filename != nullptr);
	assert(buffer != nullptr);
	
	FILE* file = nullptr;
	if (fopen_s(&file, filename, "rb") != 0)
	{
		Info("Error: Could not open file %s\n", filename);
		return 0;
	}
	
	size_t read = fread(buffer, 1, bufferSize, file);
	if (!feof(file))
	{
		Info("Error: could not read the whole program into buffer of size %zu\n", bufferSize);
		return 0;
	}

	if (read == 0)
	{
		Info("Error: unable to read program %s\n", filename); 
		return 0;
	}
	else
	{
		Info("Read program %s (%zu)", filename, read);
	}

	return read;
}

void Chip8::EmulateCycle()
{
	// fetch and execute the next op code
	unsigned short opCode = (_memory[_pc] << 8 | _memory[_pc + 1]);

	// execute the instruction
	//Info("processing opCode %04x...", opCode);
	switch (opCode & 0xF000)
	{
		case 0x0000:
		{
			switch(opCode & 0x00FF)
			{
				// 00E0 	Display 	disp_clear() 	Clears the screen
				case 0x00E0:
				{
					memset(_gfx, 0, ScreenWidth * ScreenHeight);
					_draw = true;
					_pc += 2;
					break;
				}
 
				// 00EE 	Flow 	return; 	Returns from a subroutine 
				case 0x00EE: 
				{
					_sp--;
					_pc = _stack[_sp];
					_pc += 2;
					break;
				}
				break;
 
				default:
				{
					Info("Error: Unknown opCode %04x!", opCode);
					break;
				}
			}
			break;
		}

		// 1NNN 	Flow 	goto NNN; 	Jumps to address NNN. 
		case 0x1000:
		{
			_pc = opCode & 0x0FFF;
			break;
		}

		// 2NNN 	Flow 	*(0xNNN)() 	Calls subroutine at NNN
		case 0x2000:
		{
			_stack[_sp++] = _pc;
			_pc = opCode & 0x0FFF;
			break;
		}

		// 3XNN 	Cond 	if(Vx==NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block) 
		case 0x3000:
		{
			unsigned short x = (opCode & 0x0F00) >> 8;
			if (_v[x] == (opCode & 0x00FF))
			{
				_pc += 2;
			}
			
			_pc += 2;
			break;
		}

		// 4XNN 	Cond 	if(Vx!=NN) 	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block) 
		case 0x4000:
		{
			unsigned short x = (opCode & 0x0F00) >> 8;
			if (_v[x] != (opCode & 0x00FF))
			{
				_pc += 2;
			}
			
			_pc += 2;
			break;
		}

		// 5XY0 	Cond 	if(Vx==Vy) 	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block) 
		case 0x5000:
		{
			unsigned short x = (opCode & 0x0F00) >> 8;
			unsigned short y = (opCode & 0x00F0) >> 4;

			if (_v[x] != _v[y])
			{
				_pc += 2;
			}
			
			_pc += 2;
			break;
		}

		// 6XNN 	Const 	Vx = NN 	Sets VX to NN
		case 0x6000:
		{
			unsigned short v = (opCode & 0x0F00) >> 8;
			unsigned char val = (opCode & 0x00FF);
			assert(v < NumRegisers);
			_v[v] = val;
			
			_pc += 2;
			break;
		}

		// 7XNN 	Const 	Vx += NN 	Adds NN to VX. (Carry flag is not changed) 
		case 0x7000:
		{
			unsigned short v = (opCode & 0x0F00) >> 8;
			unsigned short val = (opCode & 0x00FF);
			_v[v] += val;

			_pc += 2;
			break;
		}

		case 0x8000:
		{
			unsigned short x = (opCode & 0x0F00) >> 8;
			unsigned short y = (opCode & 0x00F0) >> 4;
			switch (opCode & 0x000F)
			{
				/*.

..
8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
8XY6[a] 	BitOp 	Vx>>=1 	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
8XY7[a] 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
8XYE[a] 	BitOp 	Vx<<=1 	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]*/

				// 8XY0 	Assign 	Vx=Vy 	Sets VX to the value of VY
				case 0x0000:
				{
					_v[x] = _v[y];
					
					_pc += 2;
					break;
				}

				// 8XY1 	BitOp 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)
				case 0x0001:
				{
					_v[x] = _v[x] | _v[y];
					
					_pc += 2;
					break;
				}

				// 8XY2 	BitOp 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)
				case 0x0002:
				{
					_v[x] = _v[x] & _v[y];
					
					_pc += 2;
					break;
				}

				// 8XY3[a] 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY
				case 0x0003:
				{
					_v[x] = _v[x] ^ _v[y];
					
					_pc += 2;
					break;
				}

				// 8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
				case 0x0004:
				{
					_v[0xF] = (_v[y] > (0xFF - _v[x])) ? 1 : 0;
					_v[x] += _v[y];
					
					_pc += 2;
					break;
				}

				// 8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				case 0x0005:
				{
					_v[0xF] = (_v[y] > _v[x]) ? 1 : 0;
					_v[x] -= _v[y];
					
					_pc += 2;
					break;
				}

				default:
				{
					Info("Error: Unknown opCode %04x!", opCode);
					break;
				}
			}

			break;
		}

		// ANNN 	MEM 	I = NNN 	Sets I to the address NNN
		case 0xA000:
		{
			_i = opCode & 0x0FFF;
			_pc += 2;
			break;
		}

		// CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN. 
		case 0xC000:
		{
			unsigned short x = (opCode & 0x0F00) >> 8;
			unsigned short r = static_cast<unsigned short>(rand() % (255 - 1)); 

			if (r < 0 || r > 255)
			{
				Info("Error: rand value out of range %u", rand);
				r = r < 0 ? 0 : r;
				r = r > 255 ? 255 : r;
			}

			_v[x] = (opCode & 0x0FFF) & r;
			_pc += 2;
			break;
		}

		// DXYN 	Disp 	draw(Vx,Vy,N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen 
		case 0xD000:
		{
			unsigned short x = _v[(opCode & 0x0F00) >> 8];
			unsigned short y = _v[(opCode & 0x00F0) >> 4];
			unsigned short height = (opCode & 0x000F);

			_v[0xF] = 0;	// clear the carry bit

			for (int pixelY = 0 ; pixelY < height ; ++pixelY)
			{
				unsigned short pixel = _memory[_i + pixelY];
				for (int pixelX = 0 ; pixelX < 8 ; ++pixelX)
				{
					if ((pixel & (0x80 >> pixelX)) != 0)	// if the pixel bit is set
					{
						int index = x + pixelX + ((y + pixelY) * ScreenWidth);

						// if the pixel is already set then set the carry flag
						if (_gfx[index] != 0)
						{
							_v[0xF] = 1;
						}

						// set the pixel
						_gfx[index] ^= 1;
					}
				}
			}

			_pc += 2;
			_draw = true;
			break;
		}

		case 0xE000:
		{
			unsigned short x = (opCode & 0x0F00) >> 8;

			switch (opCode & 0x00FF)
			{
				// EX9E 	KeyOp 	if(key()==Vx) 	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
				case 0x009E:
				{
					if (_keys[_v[x]] != 0)
					{
						_pc += 2;
					}

					break;
				}
				
				// EXA1 	KeyOp 	if(key()!=Vx) 	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block) 
				case 0x00A1:
				{
					if (_keys[_v[x]] == 0)
					{
						_pc += 2;
					}

					break;
				}

				default:
				{
					Info("Error: Unknown opCode %04x!", opCode);
					break;
				}
			}

			_pc += 2;
			break;
		}

		case 0xF000:
		{

//FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)

//FX1E 	MEM 	I +=Vx 	Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.[c]
//FX55 	MEM 	reg_dump(Vx,&I) 	Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]

			unsigned short x = (opCode & 0x0F00) >> 8;
			switch (opCode & 0x00FF)
			{
				// FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
				case 0x0007:
				{
					_v[x] = _delayTimer;
					_pc += 2;
					break;
				}

				// FX33 	BCD 	set_BCD(Vx); Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.) 
				case 0x0033:
				{
					_memory[_i] = _v[x] / 100;
					_memory[_i + 1] = (_v[x] / 10) % 10;
					_memory[_i + 2] = (_v[x] % 100) % 10;

					_pc += 2;
					break;
				}

				// FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
				/*case 0x000A:
				{

					break;
				}*/

				// FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX
				case 0x0015:
				{
					_delayTimer = _v[x];
					
					_pc += 2;
					break;
				}

				// FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX
				case 0x0018:
				{
					_soundTimer = _v[x];
					
					_pc += 2;
					break;
				}

				// FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
				case 0x0029:
				{
					_i = FontStart + (_v[x] * 5);	// font is stored at FontStart
					_pc += 2;
					break;
				}

				// FX65 	MEM 	reg_load(Vx,&I) 	Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
				case 0x0065:
				{
					for (unsigned v = 0 ; v <= x ; ++v)
					{
						_v[v] = _memory[_i + v];
					}

					_pc += 2;
					break;
				}

				default:
				{
					Info("Error: Unknown opCode %04x!", opCode);
					break;
				}
			}

			break;
		}
			
		default:
		{
			Info("Error: Unknown opCode %04x!", opCode);
			break;
		}
	}

	if (_delayTimer > 0)
	{
		--_delayTimer;
	}
 
	if (_soundTimer > 0)
	{
		if(_soundTimer == 1)
		{
			Info("BEEP!");
		}
		--_soundTimer;
	}  
}