#version 440 core

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec4 worldPosition;
    vec2 texCoord;
    vec3 normal;
} vs_out;

void main()
{
    vs_out.worldPosition = perObjectUpdate.transformationMatrix * vec4(POSITION, 1.0);
    vs_out.texCoord      = TEXTCOORD;
    vs_out.normal        = (perObjectUpdate.transformationMatrix * vec4(NORMAL, 0.0)).xyz;
    gl_Position = perFrame.projectionViewMatrix * vs_out.worldPosition;
}