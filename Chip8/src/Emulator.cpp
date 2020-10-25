#include "pch.h"

#include "Emulator.h"

#include "Log.h"

namespace chip8
{
	Emulator::Emulator()
		: _programFile("../assets/programs/Original/pong.ch8")
	{
		_chip8.Initialize();
		_chip8.LoadProgram(_programFile.c_str());
	
		_program = new unsigned char[Chip8::TotalMemoryBytes];
		_programSize = Chip8::ReadProgram(_programFile.c_str(), _program, Chip8::TotalMemoryBytes);	
	}

	Emulator::~Emulator()
	{
	}

    void Emulator::Update(float delta)
	{
		if (_paused)
		{
			// emulate a single cycle if we step while paused
			if (_step > 0)
			{
				_chip8.EmulateCycle();
				--_step;
			}
		}
		else 
		{
			// emulate N cycles
			float clockSpeed = static_cast<float>(Chip8::ClockSpeed) * _clockScale;
			int numCycles = static_cast<int>(delta * clockSpeed);
			if (numCycles <= 0)
			{
				numCycles = 1;
			}

			while (numCycles > 0)
			{
				_chip8.EmulateCycle();
				--numCycles;
			}
		}
	}

	// TODO - this probably should not have an unchecked pointer passed in
	bool Emulator::Draw(unsigned int scale, unsigned char* gfxTexture)
	{
		if (!_chip8.Draw())
		{
			return false;
		}

		size_t bufferSize = ((Chip8::ScreenWidth * scale) * (Chip8::ScreenHeight * scale)) * 4;
		memset(gfxTexture, 0x00, bufferSize);
		
		// draw any on pixels into the buffer
		for (unsigned int y = 0 ; y < Chip8::ScreenHeight ; ++y)
		{
			for (unsigned int x = 0 ; x < Chip8::ScreenWidth ; ++x)
			{
				unsigned char pixel = _chip8.Pixel(x, y);
				if (pixel != 0)
				{
					DrawPixel(x, y, scale, true, gfxTexture);
				}
			}
		}

		_chip8.ClearDraw();
		return true;
	}

	void Emulator::DrawPixel(unsigned int x, unsigned int y, unsigned int scale, bool filled, unsigned char* gfxTexture)
	{
		unsigned int startX = (scale * x);
		unsigned int startY = (scale * y);
		for (unsigned int pixelY = startY ; pixelY < (startY + scale) ; ++pixelY)
		{
			for (unsigned int pixelX = startX ; pixelX < (startX + scale) ; ++pixelX)
			{
				unsigned int pixelIdx = ((pixelY * (Chip8::ScreenWidth * scale)) * 4) + (pixelX * 4);
				gfxTexture[pixelIdx] = 0xFF;
				gfxTexture[pixelIdx + 1] = 0xFF;
				gfxTexture[pixelIdx + 2] = 0xFF;
				gfxTexture[pixelIdx + 3] = 0xFF;
			}
		}
	}

	void Emulator::OnKeyUp(unsigned int key)
	{
		_chip8.KeyUp(key);
	}

	void Emulator::OnKeyDown(unsigned int key)
	{
		_chip8.KeyDown(key);
	}

	void Emulator::OnGui(unsigned int width, unsigned int height)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(width), static_cast<float>(height)));
	
		int windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		windowFlags |= ImGuiWindowFlags_NoDecoration;
		windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin("##background", nullptr, windowFlags);
		ImGui::End();

		ImGui::PopStyleVar(1);

		if (ImGui::Begin("System"))
		{
			ImGui::Text("Program: %s", _programFile.c_str());
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			ImGui::Text("Clock Speed:");
			ImGui::SameLine();
			ImGui::SliderFloat("##clockscale", &_clockScale, 0.1f, 5.0f);
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				_clockScale = 1.0f;
			}
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
				_paused = true;
				_chip8.Initialize();
				_chip8.LoadProgram(_programFile.c_str());
			}
			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				ImGui::OpenPopup("Load Program");	
				ImGui::SetNextWindowSize(ImVec2(320, 480));
			}

			static bool LoadOpen = true;
		
			if (ImGui::BeginPopupModal("Load Program", &LoadOpen))
			{
				static std::string CurrPath = "../assets/programs/Chip-8 Games/";
			
				typedef std::vector<std::string> FileList;
				static FileList Files;

				if (Files.size() == 0)
				{
					Files.push_back("..");
					for (const auto& entry : std::filesystem::directory_iterator(CurrPath))
					{
						//Info(entry.path().extension().string().c_str());
						if (entry.path().extension() == ".ch8")
						{
							Files.push_back(entry.path().filename().string());
						}
					}
				}

				static int Selected = 0;

				ImGui::BeginChild("files", ImVec2(0, -25), true);

				int index = 0;
				bool load = false;
				for (std::string& file : Files)
				{
					if (ImGui::Selectable(file.c_str(), index == Selected))
					{
						Selected = index;	
					}

					if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0))
					{
						load = true;
					}

					++index;
				}

				ImGui::EndChild();
				if (ImGui::Button("Load") || load)
				{
					_programFile = CurrPath + Files[Selected];
	
					_chip8.Initialize();
					_chip8.LoadProgram(_programFile.c_str());
					_paused = false;

					_program = new unsigned char[Chip8::TotalMemoryBytes];
					_programSize = Chip8::ReadProgram(_programFile.c_str(), _program, Chip8::TotalMemoryBytes);
					if (_programSize == 0)
					{
						logging::Info("Error: could not read program");
					}	
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();   
			}

			if (ImGui::CollapsingHeader("Stack", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent();

				int count = 0;
				for (unsigned short sp = _chip8.StackPointer() ; sp > 0 ; --sp, ++count)
				{
					ImGui::Text("%u:\t0x%04X", count + 1, _chip8.Stack()[sp - 1]);
				}

				ImGui::Unindent();
			}

			ImGui::BeginChild("instructions");

			for (unsigned short pc = 0 ; pc < _programSize ; pc += 2)
			{
				bool active = pc == (_chip8.PC() - Chip8::ProgramStart);
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
			ImGui::Text("I:\t\t0x%02X", _chip8.I());
			ImGui::Text("Delay:\t0x%02X", _chip8.DelayTimer());
			ImGui::Text("Sound:\t0x%02X", _chip8.SoundTimer());

			ImGui::Separator();
			ImGui::Columns(4, 0, false);
			for (unsigned int reg = 0 ; reg < Chip8::NumRegisers ; ++reg)
			{
				ImGui::Text("V%X:\t0x%02X", reg, _chip8.Register(reg));
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
								_chip8.Memory(loc), 
								_chip8.Memory(loc + 1), 
								_chip8.Memory(loc + 2), 
								_chip8.Memory(loc + 3));
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
					line[x] = _chip8.Pixel(x, y) == 0 ? '0' : '1';
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
						_chip8.KeyUp(key);
					}
					else if (ImGui::IsItemClicked(0))
					{
						//Info("KeyDown %X", key);
						_chip8.KeyDown(key);
					}
				
					if (KeyDebug)
					{
						ImGui::SameLine();	
						ImGui::Text(_chip8.Key(key) ? "*" : "-");
					}

					if (col < (numCols - 1))
					{
						ImGui::SameLine();
					}
				}
			}
		}
		ImGui::End();

		
	}
}