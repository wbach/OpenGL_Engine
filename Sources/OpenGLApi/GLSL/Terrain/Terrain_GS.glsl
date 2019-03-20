#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out; //line_strip

uniform mat4 m_ViewProjection;

in vec2 mapCoord_GS[];

out vec4 worldPos;
out vec2 mapCoord_FS;

void main()
{
    for (int i = 0; i < gl_in.length(); ++i)
    {
        vec4 position = gl_in[i].gl_Position;
        mapCoord_FS = mapCoord_GS[i];
        worldPos =  m_ViewProjection * position;
        gl_Position = worldPos;
        EmitVertex();
    }
    EndPrimitive();
}