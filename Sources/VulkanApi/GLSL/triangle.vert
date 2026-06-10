#version 450

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = vec4(Position, 1.0);
    fragColor = vec3(TexCoord, 0.0);
}