#version 330

layout (location = 0) in vec3 Position;

out vec3 TextureCoords;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * vec4(Position, 1.f);
	TextureCoords = Position;
}