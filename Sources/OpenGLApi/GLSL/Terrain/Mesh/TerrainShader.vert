#version 330

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float useShadows;
uniform float shadowDistance;
uniform float shadowMapSize;
uniform mat4 toShadowMapSpace;
uniform float useNormalMap;

const float TRANSITION_DISTANCE = 2.f;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    bool useShadows;
    float shadowMapSize;
    vec3 passTangent;
    bool useNormalMap;
} vs_out;

bool convert(float f)
{
    return f > .5f;
}

void main()
{
    vec4 worldPos           = transformMatrix* vec4(POSITION, 1.0);
    vec4 modelViewPosition  = viewMatrix * worldPos;
    vs_out.texCoord         = TEXTCOORD;
    vs_out.normal           = (transformMatrix * vec4(NORMAL, 0.0)).xyz;
    vs_out.worldPos         = worldPos;

    if( useNormalMap > .5f) 
    {
        vs_out.passTangent  = (transformMatrix * vec4(TANGENT, 0.0)).xyz; 
        vs_out.useNormalMap = true;
    }
    else
    {
        vs_out.useNormalMap = false;
        vs_out.passTangent  = vec3(.0f) ;
    }

    float distanceToCam = length(modelViewPosition.xyz);
    vs_out.useShadows    = convert(useShadows);

    if (vs_out.useShadows)
    {
        vs_out.shadowMapSize  = shadowMapSize;
        vs_out.shadowCoords   = toShadowMapSpace * vec4(vs_out.worldPos.xyz, 1.f); 
        vs_out.shadowCoords.w = (distanceToCam - (shadowDistance - TRANSITION_DISTANCE)) / shadowDistance;
        vs_out.shadowCoords.w = clamp(1.f - vs_out.shadowCoords.w, 0.f, 1.f);
    }

    gl_Position = projectionMatrix * modelViewPosition;
}