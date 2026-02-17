#version 450
#extension GL_GOOGLE_include_directive : enable
#include "../../Common/PerApp.glsl"
#include "../../Common/PerFrameBuffer.glsl"

const float TRANSITION_DISTANCE = 2.f;

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
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
    float visibility;
} vs_out;

bool Is(float f)
{
    return f > .5f;
}

mat3 CreateTBNMatrix(mat4 normalTransformationMatrix, vec3 normal)
{
    vec3 tangent  = normalize((normalTransformationMatrix * vec4(TANGENT, 0.0)).xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 binormal = cross(normal, tangent);
    return mat3(tangent, binormal, normal);
}

void main()
{
    mat4 normalTransformationMatrix = transpose(inverse(perObjectUpdate.transformationMatrix));
    vs_out.worldPos    = perObjectUpdate.transformationMatrix * vec4(POSITION, 1.0);
    vs_out.normal      = normalize(( normalTransformationMatrix * vec4(NORMAL, 0.0)).xyz);
    vs_out.texCoord    = TEXTCOORD;

    if (Is(perApp.useTextures.y))
    {
        vs_out.tbn = CreateTBNMatrix(normalTransformationMatrix, vs_out.normal);
    }

    gl_Position = perFrame.projectionViewMatrix * vs_out.worldPos;

    float distance = length(perFrame.cameraPosition - vs_out.worldPos.xyz);
    float visibility = exp(-pow((distance*( ( 3.0f / perApp.viewDistance.x))), perApp.fogData.w));
    vs_out.visibility = clamp(visibility, 0.f, 1.f);
}