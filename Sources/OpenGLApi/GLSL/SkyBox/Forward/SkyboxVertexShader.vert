#version 440 core

layout (location = 0) in vec3 Position;

out vec3 TextureCoords;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
    vec4 time;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

void main(void)
{
    vec4 worldPosition = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    gl_Position = perFrame.projectionViewMatrix * worldPosition;
    gl_ClipDistance[0] = dot(worldPosition, perFrame.clipPlane);
    TextureCoords = Position;
}