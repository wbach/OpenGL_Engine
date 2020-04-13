#version 440 core

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

out vec3 color;

void main()
{
    gl_Position = perFrame.projectionViewMatrix * vec4(Position, 1.f);
    color = Color;
}