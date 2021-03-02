#version 440 core
const int MAX_SHADOW_MAP_CASADES = 4;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
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
    float visibility;
} gs_in[];

out GS_OUT
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
    vec3 faceNormal;
    float visibility;
} gs_out;

vec3 calculateFaceNormal()
{
    vec3 v1 = gs_in[1].worldPos.xyz - gs_in[0].worldPos.xyz;
    vec3 v2 = gs_in[2].worldPos.xyz - gs_in[0].worldPos.xyz;
    return normalize(cross(v1, v2));
}

void main()
{
    vec3 faceNormal = calculateFaceNormal();

    for (int i = 0; i < gl_in.length(); ++i)
    {
        gs_out.texCoord         = gs_in[i].texCoord;
        gs_out.normal           = gs_in[i].normal;
        gs_out.worldPos         = gs_in[i].worldPos;
        gs_out.positionInLightSpace  = gs_in[i].positionInLightSpace;
        gs_out.useShadows       = gs_in[i].useShadows;
        gs_out.shadowTransition = gs_in[i].shadowTransition;
        gs_out.shadowMapSize    = gs_in[i].shadowMapSize;
        gs_out.clipSpaceZ       = gs_in[i].clipSpaceZ;
        gs_out.tbn              = gs_in[i].tbn;
        gs_out.visibility       = gs_in[i].visibility;
        gs_out.faceNormal       = faceNormal;
        gl_Position             = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}