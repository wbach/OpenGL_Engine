#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in mat4 ModelViewMatrix;

uniform mat4 ProjectionMatrix;

out vec2 textureCoords;

void main(void)
{
    textureCoords = TexCoord;
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(Position, 1.0);
}