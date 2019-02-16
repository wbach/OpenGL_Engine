#version 440

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

layout (std140, align=16, binding=0) uniform PerApp
{
    float useTextures;
    float viewDistance;
    vec3 shadowVariables;
    vec4 clipPlane;
} perApp;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

const float TRANSITION_DISTANCE = 2.f;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    vec3 passTangent;
    float useNormalMap;
} vs_out;

bool Is(float f)
{
    return f > .5f;
}

void main()
{
    vec4 worldPos           = perObjectUpdate.transformationMatrix * vec4(POSITION, 1.0);
    vs_out.texCoord         = TEXTCOORD;
    vs_out.normal           = (perObjectUpdate.transformationMatrix * vec4(NORMAL, 0.0)).xyz;
    vs_out.worldPos         = worldPos;

    vs_out.passTangent  = (perObjectUpdate.transformationMatrix * vec4(TANGENT, 0.0)).xyz; 
    vs_out.useNormalMap = 0.f;

    float distanceToCam = length(perFrame.cameraPosition - worldPos.xyz);
    vs_out.useShadows    = perApp.shadowVariables.x;

    if (Is(vs_out.useShadows))
    {
        vs_out.shadowMapSize  = perApp.shadowVariables.z;
        vs_out.shadowCoords   = perFrame.toShadowMapSpace * vec4(vs_out.worldPos.xyz, 1.f); 
        vs_out.shadowCoords.w = (distanceToCam - (perApp.shadowVariables.y - TRANSITION_DISTANCE)) / perApp.shadowVariables.y;
        vs_out.shadowCoords.w = clamp(1.f - vs_out.shadowCoords.w, 0.f, 1.f);
    }

    gl_Position = perFrame.projectionViewMatrix * worldPos;
}