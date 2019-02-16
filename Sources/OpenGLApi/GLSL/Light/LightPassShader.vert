#version 420

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

out VS_OUT
{
    vec2 textureCoords;
    vec3 cameraPosition;
} vs_out;

void main()
{
    vs_out.textureCoords = TexCoord;
    vs_out.cameraPosition = perFrame.cameraPosition;
    gl_Position = vec4(Position, 1.0);
}
