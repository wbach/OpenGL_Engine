#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    mat3 tbn;
} gs_in[];

out GS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    mat3 tbn;
    vec3 faceNormal;
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
        gs_out.texCoord       = gs_in[i].texCoord;
        gs_out.normal         = gs_in[i].normal;
        gs_out.worldPos       = gs_in[i].worldPos;
        gs_out.shadowCoords   = gs_in[i].shadowCoords;
        gs_out.useShadows     = gs_in[i].useShadows;
        gs_out.shadowMapSize  = gs_in[i].shadowMapSize;
        gs_out.tbn            = gs_in[i].tbn;
        gs_out.faceNormal     = faceNormal; 
        gl_Position           = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}