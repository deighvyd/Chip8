#include "pch.h"

#include "Designer.h"

#include "Chip8.h"
#include "Log.h"

#include "imgui/imgui.h"

using namespace logging;

namespace detail
{
	static const int ClokSpeed = 500;	// Hz
	static const int ClockScale = 1;
}

/*static */int Designer::DisplayWidth()
{
	return (Chip8::ScreenWidth * DisplayScale);
}

/*static */int Designer::DisplayHeight()
{
	return (Chip8::ScreenHeight * DisplayScale);
}

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

	delete _gfxTexture;
	_gfxTexture = nullptr;

	// TODO - release the texture memory?????
}

bool Designer::Initialize(LPCWSTR name)
{
	if (!Application::Initialize(name))
	{
		return false;
	}

	assert(_gfxTexture == nullptr);

	// allocate a buffer for the texture data
	size_t bufferSize = (DisplayWidth() * DisplayHeight() * 4);
	_gfxTexture = new unsigned char[bufferSize];
	if (_gfxTexture == nullptr)
	{
		Info("Error: could not allocate memory for the gfx texture");
		return false;
	}
	memset(_gfxTexture, 0x00, bufferSize);

	UploadGfxTexture();

	return true;
}

bool Designer::UpdateGfxTexture()
{
	// clear the texture
	size_t bufferSize = (DisplayWidth() * DisplayHeight()) * 4;
	memset(_gfxTexture, 0x00, bufferSize);

	// draw any on pixels into the buffer
	for (int y = 0 ; y < Chip8::ScreenHeight ; ++y)
	{
		for (int x = 0 ; x < Chip8::ScreenWidth ; ++x)
		{
			unsigned char pixel = _chip8->Pixel(x, y);
			if (pixel != 0)
			{
				DrawPixel(x, y, true);
			}
		}
	}

	UploadGfxTexture();

	return true;
}

void Designer::UploadGfxTexture()
{
	// TODO - release the old texture

	// build the texture
	GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	_gfxTextureId = (ImTextureID)texture;

    // setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    // upload pixels into texture
	// TODO - allow this to be regenerated
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DisplayWidth(), DisplayHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _gfxTexture);
}

void Designer::DrawPixel(int x, int y, bool filled)
{
	int startX = (DisplayScale * x);
	int startY = (DisplayScale * y);
	for (int pixelY = startY ; pixelY < (startY + (int)DisplayScale) ; ++pixelY)
	{
		for (int pixelX = startX ; pixelX < (startX + (int)DisplayScale) ; ++pixelX)
		{
			int pixelIdx = ((pixelY * DisplayWidth()) * 4) + (pixelX * 4);
			_gfxTexture[pixelIdx] = 0xFF;
			_gfxTexture[pixelIdx + 1] = 0xFF;
			_gfxTexture[pixelIdx + 2] = 0xFF;
			_gfxTexture[pixelIdx + 3] = 0xFF;
		}
	}
}

void Designer::OnUpdate(float delta)
{
	Application::OnUpdate(delta);
	
	if (_paused)
	{
		// emulate a single cycle if we step while paused
		if (_step > 0)
		{
			_chip8->EmulateCycle();
			--_step;
		}
	}
	else 
	{
		// emulate N cycles
		int clockSpeed = (detail::ClokSpeed * detail::ClockScale);
		int numCycles = static_cast<int>(delta * static_cast<float>(clockSpeed));
		while (numCycles > 0)
		{
			_chip8->EmulateCycle();
			--numCycles;
		}
	}
}

void Designer::OnDraw()
{
	Application::OnDraw();

	if (_chip8->Draw())
	{
		if (!UpdateGfxTexture())
		{
			MessageBox(_hWnd, L"Failed to update the gfx texture", L"Error", MB_OK);
			// not fatal so do no bail
		}
		_chip8->ClearDraw();
	}
}

void Designer::OnGui()
{
	if (ImGui::Begin("Info"))
	{
		ImGui::Text("Program: %s", _programFile);
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	}
	ImGui::End();

	if (ImGui::Begin("Program"))
	{
		if (ImGui::Button(_paused ? "Play" : "Pause"))
		{
			_paused = !_paused;
			_step = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			if (_paused)
			{
				++_step;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			_chip8->Initialize();
			_chip8->LoadProgram(_programFile);
		}

		if (ImGui::CollapsingHeader("Stack", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent();

			int count = 0;
			for (unsigned short sp = _chip8->StackPointer() ; sp > 0 ; --sp, ++count)
			{
				ImGui::Text("%u:\t0x%04X", count + 1, _chip8->Stack()[sp - 1]);

			}

			ImGui::Unindent();
		}

		assert((_programSize % 2) == 0);
		ImGui::BeginChild("instructions");

		for (unsigned short pc = 0 ; pc < _programSize ; pc += 2)
		{
			bool active = pc == (_chip8->PC() - Chip8::ProgramStart);
			if (active)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 1.0f, 0.0f, 1.0f });
			}

			unsigned short opCode = (_program[pc] << 8 | _program[pc + 1]);
			ImGui::Text("%04d (0x%04X):\t0x%04X", (pc / 2) + 1, Chip8::ProgramStart + pc, opCode);

			if (active)
			{
				ImGui::SetScrollHere();
				ImGui::PopStyleColor(1);
			}
		}

		ImGui::EndChild();
	}
	ImGui::End();

	if (ImGui::Begin("Registers"))
	{
		// TODO - binary view?
		ImGui::Text("I:\t\t0x%02X", _chip8->I());
		ImGui::Text("Delay:\t0x%02X", _chip8->DelayTimer());
		ImGui::Text("Sound:\t0x%02X", _chip8->SoundTimer());

		ImGui::Separator();
		ImGui::Columns(4, 0, false);
		for (unsigned int reg = 0 ; reg < Chip8::NumRegisers ; ++reg)
		{
			ImGui::Text("V%X:\t0x%02X", reg, _chip8->Register(reg));
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
	}
	ImGui::End();

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
	}
	ImGui::End();

	if (ImGui::Begin("Gfx"))
	{
		for (unsigned int y = 0 ; y < Chip8::ScreenHeight ; ++y)
		{
			char line[Chip8::ScreenWidth + 1];
			for (unsigned int x = 0 ; x < Chip8::ScreenWidth ; ++x)
			{
				line[x] = _chip8->Pixel(x, y) == 0 ? '0' : '1';
			}
			line[Chip8::ScreenWidth] = '\0';
			ImGui::Text("%s", line);
		}
	}
	ImGui::End();

	if (ImGui::Begin("Keypad"))
	{
		static constexpr unsigned char KeyLayout[Chip8::NumKeys] = 
		{
			0x1, 0x2, 0x3, 0xC,
			0x4, 0x5, 0x6, 0xD,
			0x7, 0x8, 0x9, 0xE,
			0xA, 0x0, 0xB, 0xF,
		};

		static bool KeyDebug = false;
		ImGui::Checkbox("Debug", &KeyDebug);

		unsigned int numCols = Chip8::NumKeys / 4;
		unsigned int numRows = Chip8::NumKeys / numCols;

		for (unsigned int row = 0 ; row < numRows ; ++row)
		{
			for (unsigned int col = 0 ; col < numCols ; ++col)
			{
				unsigned int key = KeyLayout[((row * numCols) + col)];

				char buffer[2];
				sprintf_s(buffer, "%X", key);
				
				bool released = ImGui::Button(buffer, { 32, 32 });
				if (released)
				{
					//Info("KeyUp   %X", key);
					_chip8->KeyUp(key);
				}
				else if (ImGui::IsItemClicked(0))
				{
					//Info("KeyDown %X", key);
					_chip8->KeyDown(key);
				}
				
				if (KeyDebug)
				{
					ImGui::SameLine();	
					ImGui::Text(_chip8->Key(key) ? "*" : "-");
				}

				if (col < (numCols - 1))
				{
					ImGui::SameLine();
				}
			}
		}
	}
	ImGui::End();

	ImVec2 gfxSize = ImVec2(Chip8::ScreenWidth * DisplayScale, Chip8::ScreenHeight * DisplayScale);

	ImGui::SetNextWindowSize(ImVec2(0, 0));
	if (ImGui::Begin("Emulator", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		
		ImGui::Image(_gfxTextureId, gfxSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(255,255,255,255));
		ImGui::End();
	}
}
