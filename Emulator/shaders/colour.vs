#version 400

in vec3 inputPosition;
in vec3 inputColour;

out vec3 colour;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	// calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;

	// store the input colour for the pixel shader to use.
	colour = inputColour;
}
