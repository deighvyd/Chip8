#pragma once

class Light
{
public:
	Light();
	Light(const Light&);
	virtual ~Light();

	void SetDiffuseColour(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);

	void GetDiffuseColour(float* colour);
	void GetDirection(float* direction);

private:
	float _diffuseColour[4];
	float _direction[3];

};