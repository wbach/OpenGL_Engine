#version 440 core
#extension GL_GOOGLE_include_directive : enable
#include "../../../Common/PerApp.glsl"
#include "../../../Common/PerFrameBuffer.glsl"

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

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