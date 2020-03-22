#version 440 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in mat4 ModelViewMatrix;

layout (std140, align=16, binding=6) uniform InputBuffer
{
    mat4 ProjectionMatrix;
    float NumberOfRows;
} inputBuffer;

out vec4 position;
out vec2 textureCoords;

void main(void)
{
    position =  inputBuffer.ProjectionMatrix * ModelViewMatrix * vec4(Position, 1.0);
    textureCoords = TexCoord;
    gl_Position = position;
}