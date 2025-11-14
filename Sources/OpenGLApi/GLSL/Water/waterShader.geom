#version 440 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 clipSpace;
} gs_in[];

out GS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 clipSpace;
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
        gs_out.clipSpace         = gs_in[i].clipSpace;
        gl_Position             = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}