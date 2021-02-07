#version 440 core
const int MAX_SHADOW_MAP_CASADES = 4;
const float TRANSITION_DISTANCE = 2.f;

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
} perApp;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140,binding=7) uniform ShadowsBuffer
{
    mat4 directionalLightSpace[MAX_SHADOW_MAP_CASADES];
    vec4 cascadesDistance;
    float cascadesSize;
} shadowsBuffer;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    float clipSpaceZ;
    float shadowTransition;
    vec4 positionInLightSpace[MAX_SHADOW_MAP_CASADES];
    float useShadows;
    float shadowMapSize;
    mat3 tbn;
} vs_out;

bool Is(float f)
{
    return f > .5f;
}

mat3 CreateTBNMatrix(vec3 normal)
{
    vec3 tangent  = normalize((perObjectUpdate.transformationMatrix * vec4(TANGENT, 0.0)).xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 binormal = cross(normal, tangent);
    return mat3(tangent, binormal, normal);
}

void main()
{
    vs_out.worldPos    = perObjectUpdate.transformationMatrix * vec4(POSITION, 1.0);
    vs_out.normal      = normalize((perObjectUpdate.transformationMatrix * vec4(NORMAL, 0.0)).xyz);
    vs_out.texCoord    = TEXTCOORD;

    if (Is(perApp.useTextures.y))
    {
        vs_out.tbn = CreateTBNMatrix(vs_out.normal);
    }

    float distanceToCam = length(perFrame.cameraPosition - vs_out.worldPos.xyz);
    vs_out.useShadows   = perApp.shadowVariables.x;

    if (Is(vs_out.useShadows))
    {
        for (int i = 0 ; i < MAX_SHADOW_MAP_CASADES ; i++)
        {
            vs_out.positionInLightSpace[i] = shadowsBuffer.directionalLightSpace[i] * vec4(vs_out.worldPos.xyz, 1.f);;
        }

        vs_out.shadowMapSize  = perApp.shadowVariables.z;
        vs_out.shadowTransition = (distanceToCam - (perApp.shadowVariables.y - TRANSITION_DISTANCE)) / perApp.shadowVariables.y;
        vs_out.shadowTransition = clamp(1.f - vs_out.shadowTransition, 0.f, 1.f);
    }

    gl_Position = perFrame.projectionViewMatrix * vs_out.worldPos;
    vs_out.clipSpaceZ = gl_Position.z;
}