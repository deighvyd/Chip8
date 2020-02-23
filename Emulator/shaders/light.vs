#version 400

in vec3 inputPosition;
in vec2 inputTexCoord;
in vec3 inputNormal;

out vec2 texCoord;
out vec3 normal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	// calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;

	// store the texture coordinates for the pixel shader.
	texCoord = inputTexCoord;

	// calculate the normal vector against the world matrix only.
	normal = mat3(worldMatrix) * inputNormal;
	normal = normalize(normal);
}
