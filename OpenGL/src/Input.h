#pragma once

class Input
{
public:
	struct MousePos
	{
		int x = 0;
		int y = 0;
	};

public:
	Input();
	Input(const Input& other);
	virtual ~Input();

	void Initialize();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);

	bool IsButtonPressed(unsigned int button) const;
	void ClearButtons();

	const MousePos& GetMousePosition() const { return _mousePos; }

	void OnMouseDown(unsigned int button);
	void OnMouseUp(unsigned int button);
	void OnMouseMove(int x, int y);

private:
	static constexpr unsigned int NumKeys = 256;
	static constexpr unsigned int NumMouseButtons = 5;

private:
	bool _keys[NumKeys];

	MousePos _mousePos;
	bool _mouseButtons[NumMouseButtons] = { false, false, false, false, false };

};
