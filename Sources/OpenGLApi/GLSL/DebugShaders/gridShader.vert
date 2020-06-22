#version 440 core

//Attributes
layout (location = 0) in vec2 position;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out vec2 textureCoords;

void main()
{
    gl_Position = perFrame.projectionViewMatrix * perObjectUpdate.transformationMatrix * vec4(position, 0.f, 1.f);
    textureCoords = vec2((position.x+1.0) / 2.0, 1 - (position.y + 1.0) / 2.0) ;
}