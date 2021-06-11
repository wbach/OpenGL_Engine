#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

out vec2 textureCoords;

void main()
{
    textureCoords = TexCoord;
    textureCoords.y = -textureCoords.y;
    gl_Position = vec4(Position, 1.0);
}
