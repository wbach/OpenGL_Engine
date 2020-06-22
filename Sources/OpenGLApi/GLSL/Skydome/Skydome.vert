#version 420
layout (location = 0) in vec3 Position;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out vec4 worldPosition;

void main()
{
    worldPosition = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    gl_Position = perFrame.projectionViewMatrix * worldPosition;
}
