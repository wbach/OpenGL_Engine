#version 440 core

layout (points) in;
layout (line_strip, max_vertices = 4) out;

out vec3 color;

in VS_OUT
{
    vec4 worldPosition;
    vec4 normal;
    vec4 tangent;
} gs_in[];


void main(void)
{
    color = vec3(0, 0, 1);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gs_in[0].normal;
    EmitVertex();

    EndPrimitive();

    color = vec3(0, 1, 0);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gs_in[0].tangent;
    EmitVertex();

    EndPrimitive();
}