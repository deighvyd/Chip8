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

void Light::SetAmbientLight(float red, float green, float blue, float alpha)
{
	_ambientLight[0] = red;
	_ambientLight[1] = green;
	_ambientLight[2] = blue;
	_ambientLight[3] = alpha;
}

void Light::GetDiffuseColour(float* colour) const
{
	colour[0] = _diffuseColour[0];
	colour[1] = _diffuseColour[1];
	colour[2] = _diffuseColour[2];
	colour[3] = _diffuseColour[3];
}

void Light::GetDirection(float* direction) const
{
	direction[0] = _direction[0];
	direction[1] = _direction[1];
	direction[2] = _direction[2];
}

void Light::GetAmbientLight(float* light) const
{
	light[0] = _ambientLight[0];
	light[1] = _ambientLight[1];
	light[2] = _ambientLight[2];
	light[3] = _ambientLight[3];
}