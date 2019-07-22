#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out; // line_strip

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

in vec2 mapCoord_GS[];

out vec4 worldPos;
out vec2 mapCoord_FS;
out vec3 tangent;

vec3 calcTangent()
{
    vec3 v0 = gl_in[0].gl_Position.xyz;
    vec3 v1 = gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz;

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec2 uv0 = mapCoord_GS[0];
    vec2 uv1 = mapCoord_GS[1];
    vec2 uv2 = mapCoord_GS[2];

    vec2 deltaUV1 = uv1 - uv0;
    vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    return normalize((e1 * deltaUV2.y - e2 * deltaUV1.y) * r);
}

void main()
{
    tangent = calcTangent();

    for (int i = 0; i < gl_in.length(); ++i)
    {
        vec4 position = gl_in[i].gl_Position;
        mapCoord_FS = mapCoord_GS[i];
        worldPos =  perFrame.projectionViewMatrix * position;
        gl_Position = worldPos;
        EmitVertex();
    }
    EndPrimitive();
}