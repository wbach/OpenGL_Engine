#version 420
layout (location = 0) in vec3 Position;

layout (std140, binding=1) uniform PerResize
{
    mat4 projectionMatrix;
} perResize;

layout (std140, binding=2) uniform PerFrame
{
    mat4 viewMatrix;
    mat4 toShadowMapSpace;
} perFrame;

layout (std140, binding=4) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out vec4 worldPosition;

void main()
{
    worldPosition = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    gl_Position = perResize.projectionMatrix * perFrame.viewMatrix * worldPosition;
}
