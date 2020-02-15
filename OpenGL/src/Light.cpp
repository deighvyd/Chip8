#include "pch.h"

#include "Light.h"

Light::Light()
{
}

Light::Light(const Light& other)
{
}

Light::~Light()
{
}

void Light::SetDiffuseColour(float red, float green, float blue, float alpha)
{
	_diffuseColour[0] = red;
	_diffuseColour[1] = green;
	_diffuseColour[2] = blue;
	_diffuseColour[3] = alpha;
}

void Light::SetDirection(float x, float y, float z)
{
	_direction[0] = x;
	_direction[1] = y;
	_direction[2] = z;
	return;
}

void Light::GetDiffuseColour(float* colour)
{
	colour[0] = _diffuseColour[0];
	colour[1] = _diffuseColour[1];
	colour[2] = _diffuseColour[2];
	colour[3] = _diffuseColour[3];
}


void Light::GetDirection(float* direction)
{
	direction[0] = _direction[0];
	direction[1] = _direction[1];
	direction[2] = _direction[2];
}