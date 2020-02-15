#pragma once

class Input
{
public:
	Input();
	Input(const Input& other);
	virtual ~Input();

	void Initialize();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);

private:
	static constexpr unsigned int NumKeys = 256;

private:
	bool _keys[NumKeys];

};
