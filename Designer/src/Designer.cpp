#include "pch.h"

#include "Designer.h"

#include "Chip8.h"

#include "imgui/imgui.h"

Designer::Designer()
	: Application(1280, 720)
{
	_programFile = "../assets/programs/pong";

	_chip8 = new Chip8(_programFile);
	
	_program = new unsigned char[Chip8::TotalMemoryBytes];
	_programSize = Chip8::ReadProgram(_programFile, _program, Chip8::TotalMemoryBytes);
	if (_programSize == 0)
	{
		MessageBox(_hWnd, L"Could not read program", L"Error", MB_OK);
		return;
	}
}

Designer::~Designer()
{
	delete _chip8;
	_chip8 = nullptr;

	delete _program;
	_program = nullptr;
}

void Designer::OnGui()
{
	if (ImGui::Begin("Program"))
	{
		assert((_programSize % 2) == 0);

		for (unsigned short pc = 0 ; pc < _programSize ; pc += 2)
		{
			unsigned short opCode = (_program[pc] << 8 | _program[pc + 1]);
			ImGui::Text("%04d:\t0x%04X", (pc / 2) + 1, opCode);
		}

		ImGui::End();
	}

	if (ImGui::Begin("Registers"))
	{
		// TODO - binary view?

		ImGui::Columns(4, 0, false);
		for (unsigned int reg = 0 ; reg < Chip8::NumRegisers ; ++reg)
		{
			ImGui::Text("V%X:\t0x%X", reg, _chip8->Register(reg));
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		
		ImGui::End();
	}
}
