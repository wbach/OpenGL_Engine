#version 440 core

const float tiling = 10.0;
const float viewDistance = 450.0 ;
const float transitionDistance = 10.0;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

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

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 toCameraVector;
    float visibility;
} vs_out;

void main(void)
{
    vs_out.texCoord       = TexCoord * tiling;
    vs_out.normal         = (perObjectUpdate.transformationMatrix * vec4(Normal, 0.0)).xyz;
    vs_out.worldPos       = (perObjectUpdate.transformationMatrix * vec4(Position, 1.0)).xyz;
    vs_out.toCameraVector = perFrame.cameraPosition - vs_out.worldPos;

    vec4 position         = perFrame.projectionViewMatrix * vec4(vs_out.worldPos, 1.0f);
    vec2 ndc              = (position.xy/position.w) /2.0 + 0.5;

    gl_Position = position;

    float distance = length(perFrame.cameraPosition - vs_out.worldPos.xyz);
    float visibility = exp(-pow((distance*( ( 3.0f / perApp.viewDistance.x))), perApp.fogData.w));
    vs_out.visibility = clamp(visibility, 0.f, 1.f);
}