#pragma once

class Input
{
public:
	struct MousePos
	{
		int x = 0;
		int y = 0;
	};

	static constexpr unsigned int NumKeys = 256;
	static constexpr unsigned int NumMouseButtons = 5;

public:
	Input();
	Input(const Input& other);
	virtual ~Input();

	void Initialize();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);
	
	void SetCharacterHandler(std::function<void(unsigned int)> handler) { _charHandler = handler; }
	void OnCharacterPressed(unsigned int c);

	bool IsButtonPressed(unsigned int button) const;
	void ClearButtons();

	const MousePos& GetMousePosition() const { return _mousePos; }

	void OnMouseDown(unsigned int button);
	void OnMouseUp(unsigned int button);
	void OnMouseMove(int x, int y);
	
private:
	bool _keys[NumKeys];
	std::function<void(unsigned int)> _charHandler;

	MousePos _mousePos;
	bool _mouseButtons[NumMouseButtons] = { false, false, false, false, false };

};
