#version 440 core

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
  	vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out vec3 color;

void main()
{
    gl_Position = perFrame.projectionViewMatrix * perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    color = Color;
}