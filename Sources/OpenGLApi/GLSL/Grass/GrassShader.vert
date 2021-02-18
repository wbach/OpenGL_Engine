#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 SizeAndRotation;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Color;

out VS_OUT
{
    vec3 worldPosition;
    vec2 sizeAndRotation;
    vec3 normal;
    vec3 color;
} vs_out;

void main()
{
    vs_out.worldPosition   = Position;
    vs_out.sizeAndRotation = SizeAndRotation;
    vs_out.normal          = Normal;
    vs_out.color           = Color;
    gl_Position = vec4(Position, 1.0f);
}
