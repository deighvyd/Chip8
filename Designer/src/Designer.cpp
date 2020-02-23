#include "pch.h"

#include "Designer.h"

#include "Chip8.h"
#include "Log.h"

#include "imgui/imgui.h"

using namespace logging;

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
	}	
}

Designer::~Designer()
{
	delete _chip8;
	_chip8 = nullptr;

	delete _program;
	_program = nullptr;

	// TODO - release the texture memory?????
}

bool Designer::Initialize(LPCWSTR name)
{
	if (!Application::Initialize(name))
	{
		return false;
	}

	if (!InitGfxTexture())
	{
		MessageBox(_hWnd, L"Failed to initialize the gfx texture", L"Error", MB_OK);
		// not fata so do no bail
	}

	return true;
}

bool Designer::InitGfxTexture()
{
	int texWidth = (Chip8::ScreenWidth * DisplayScale);
	int texHeight = (Chip8::ScreenHeight * DisplayScale);

	size_t bufferSize = (texWidth * texHeight) * 4;
	unsigned char* data = new unsigned char[bufferSize];
	if (data == nullptr)
	{
		Info("Error: could not allocate memory for the gfx texture");
		return false;
	}
	memset(data, 0x00000000, bufferSize);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);\
	_gfxTextureId = (ImTextureID)texture;

    // setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    // upload pixels into texture
	// TODO - allow this to be regenerated
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	delete data;
	
	return true;
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
			ImGui::Text("V%X:\t0x%02X", reg, _chip8->Register(reg));
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		
		ImGui::End();
	}

	if (ImGui::Begin("Memory"))
	{
		// TODO - binary view?

		static constexpr int BytesPerRow = 16;
		
		unsigned int numColumns = (BytesPerRow / 4);
		unsigned int numRows = Chip8::TotalMemoryBytes / BytesPerRow;

		ImGui::Columns(numColumns + 1, 0, false);
		for (unsigned int row = 0 ; row < numRows ; ++row)
		{
			ImGui::Text("%04X:", (row * BytesPerRow));
			ImGui::NextColumn();

			for (unsigned int col = 0 ; col < numColumns; ++col)
			{
				unsigned int loc = (row * BytesPerRow) + col;
				ImGui::Text("0x%02X%02X%02X%02X", 
							_chip8->Memory(loc), 
							_chip8->Memory(loc + 1), 
							_chip8->Memory(loc + 2), 
							_chip8->Memory(loc + 3));
				ImGui::NextColumn();
			}
		}
		ImGui::Columns(1);
		
		ImGui::End();
	}

	if (ImGui::Begin("Emulator"))
	{
		ImGui::Image(_gfxTextureId, { Chip8::ScreenWidth * DisplayScale, Chip8::ScreenHeight * DisplayScale});
		ImGui::End();
	}
}
