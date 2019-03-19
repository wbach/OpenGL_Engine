#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out; //line_strip

uniform mat4 m_ViewProjection;

in vec2 mapCoord_GS[];
out vec4 worldPos;

void main()
{
    for (int i = 0; i < gl_in.length(); ++i)
    {
        vec4 position = gl_in[i].gl_Position;
        gl_Position = m_ViewProjection * position;
        EmitVertex();
    }

    vec4 position = gl_in[0].gl_Position;
    worldPos = m_ViewProjection * position;
    gl_Position = worldPos;
    EmitVertex();

    EndPrimitive();
}