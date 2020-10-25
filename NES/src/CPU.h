#pragma once

// 2A03 CPU
//
// Implements the NES's 2A03 CPU, a modified 6502
// Based on http://nesdev.com/6502.txt & http://nesdev.com/NESDoc.pdf

namespace nes
{
	class CPU
	{
	private:
		unsigned short _pc;		// Program Counter
		unsigned char _sp;		// Stack Pointer
		unsigned char _p;		// Processor Status
		
		unsigned char _a;		// Accumulator 
		unsigned char _x;		// Index Register X
		unsigned char _y;		// Index Register Y


	};
}