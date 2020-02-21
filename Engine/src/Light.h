#pragma once

class Light
{
public:
	Light();
	Light(const Light&);
	virtual ~Light();

	void SetDiffuseColour(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);
	void SetAmbientLight(float red, float green, float blue, float alpha);

	void GetDiffuseColour(float* colour) const;
	void GetDirection(float* direction) const;
	void GetAmbientLight(float* light) const;

private:
	float _diffuseColour[4];
	float _direction[3];
	float _ambientLight[4];

};