#include "pch.h"

#include "eParrot.h"

#include "IEmulator.h"
#include "Emulator.h"

#include "Log.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	eParrot* ep = new eParrot();
	if (ep->Initialize(L"eParrot"))
	{
		ep->Run();
	}

	ep->Shutdown();
	delete ep;

	return 0;
}

namespace detail
{
	std::unordered_map<WPARAM, unsigned char> KeyMap = 
	{
		{ '1', 0x1 }, { '2', 0x2 }, { '3', 0x3 }, { '4', 0xC },
		{ 'Q', 0x4 }, { 'W', 0x5 }, { 'E', 0x6 }, { 'R', 0xD },
		{ 'A', 0x7 }, { 'S', 0x8 }, { 'D', 0x9 }, { 'F', 0xE },
		{ 'Z', 0xA }, { 'X', 0x0 }, { 'C', 0xB }, { 'V', 0xF },
	};
}

unsigned int eParrot::DisplayWidth()
{
	return _emulator == nullptr ? 0 : (_emulator->GetScreenWidth() * DisplayScale);
}

unsigned int eParrot::DisplayHeight()
{
	return _emulator == nullptr ? 0 : (_emulator->GetScreenHeight() * DisplayScale);
}

eParrot::eParrot()
	: Application(1280, 720)
{
	_emulator = new chip8::Emulator();	
}

eParrot::~eParrot()
{
	delete _emulator;
	_emulator = nullptr;

	delete _gfxTexture;
	_gfxTexture = nullptr;

	// TODO - release the texture memory?????
}

bool eParrot::Initialize(LPCWSTR name)
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
		logging::Info("Error: could not allocate memory for the gfx texture");
		return false;
	}
	memset(_gfxTexture, 0x00, bufferSize);

	UploadGfxTexture();

	return true;
}

void eParrot::UploadGfxTexture()
{
	// TODO - release the old texture

	// build the texture
    glGenTextures(1, &_gfxTextureId);
    glBindTexture(GL_TEXTURE_2D, _gfxTextureId);

    // setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    // upload pixels into texture
	// TODO - allow this to be regenerated
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DisplayWidth(), DisplayHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _gfxTexture);
}

void eParrot::OnKeyDown(WPARAM key)
{
	if (_emulator == nullptr)
	{
		return;
	}

	if (detail::KeyMap.find(key) != detail::KeyMap.end())
	{
		_emulator->OnKeyDown(detail::KeyMap[key]);
	}
}

void eParrot::OnKeyUp(WPARAM key)
{
	if (_emulator == nullptr)
	{
		return;
	}

	if (detail::KeyMap.find(key) != detail::KeyMap.end())
	{
		_emulator->OnKeyUp(detail::KeyMap[key]);
	}
}

void eParrot::OnUpdate(float delta)
{
	Application::OnUpdate(delta);
	
	if (_emulator == nullptr)
	{
		return;
	}

	_emulator->Update(delta);
}

void eParrot::OnDraw()
{
	Application::OnDraw();
	
	if (_emulator == nullptr)
	{
		return;
	}

	_emulator->Draw(DisplayScale, _gfxTexture);

	UploadGfxTexture();
}

void eParrot::OnGui()
{
	Application::OnGui();

	if (_emulator == nullptr)
	{
		return;
	}

	ImVec2 gfxSize = ImVec2(static_cast<float>(DisplayWidth()), static_cast<float>(DisplayHeight()));

	ImGui::SetNextWindowSize(ImVec2(0, 0));
	if (ImGui::Begin("Emulator", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{	
		ImGui::Image((ImTextureID)(uintptr_t)_gfxTextureId, gfxSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(255,255,255,255));
		ImGui::End();
	}

	_emulator->OnGui(_width, _height);
}
