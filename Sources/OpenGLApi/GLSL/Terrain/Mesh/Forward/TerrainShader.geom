#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

in VS_OUT
{
    vec4 worldPosition;
    vec2 texCoord;
    vec3 normal;
    float visibility;
} gs_in[];

out GS_OUT
{
    vec4 worldPosition;
    vec2 texCoord;
    vec3 normal;
    vec3 faceNormal;
    float visibility;
} gs_out;

vec3 calculateFaceNormal()
{
    vec3 v1 = gs_in[1].worldPosition.xyz - gs_in[0].worldPosition.xyz;
    vec3 v2 = gs_in[2].worldPosition.xyz - gs_in[0].worldPosition.xyz;
    return normalize(cross(v1, v2));
}

void main()
{
    vec3 faceNormal = calculateFaceNormal();

    for (int i = 0; i < gl_in.length(); ++i)
    {
        gs_out.worldPosition  = gs_in[i].worldPosition;
        gs_out.texCoord       = gs_in[i].texCoord;
        gs_out.normal         = gs_in[i].normal;
        gs_out.visibility     = gs_in[i].visibility;
        gs_out.faceNormal     = faceNormal;
        gl_ClipDistance[0]    = dot(gs_in[i].worldPosition, perFrame.clipPlane);
        gl_Position           = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}