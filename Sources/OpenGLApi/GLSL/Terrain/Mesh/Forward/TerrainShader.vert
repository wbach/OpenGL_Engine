#version 440 core

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

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

out VS_OUT
{
    vec4 worldPosition;
    vec2 texCoord;
    vec3 normal;
    float visibility;
} vs_out;


void main()
{
    mat4 normalTransformationMatrix = transpose(inverse(perObjectUpdate.transformationMatrix));
    vs_out.worldPosition = perObjectUpdate.transformationMatrix * vec4(POSITION, 1.0);
    vs_out.texCoord      = TEXTCOORD;
    vs_out.normal        = normalize(( normalTransformationMatrix * vec4(NORMAL, 0.0)).xyz);
    gl_Position = perFrame.projectionViewMatrix * vs_out.worldPosition;

    float distance = length(perFrame.cameraPosition - vs_out.worldPosition.xyz);
    float visibility = exp(-pow((distance*( ( 3.0f / perApp.viewDistance.x))), perApp.fogData.w));
    vs_out.visibility = clamp(visibility, 0.f, 1.f);
}