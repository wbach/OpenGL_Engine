#version 420 core

out VS_OUT
{
    vec2 tc;
} vs_out;

uniform mat4 transformMatrix;

void main(void)
{
    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
                                   vec4( 0.5, 0.0, -0.5, 1.0),
                                   vec4(-0.5, 0.0,  0.5, 1.0),
                                   vec4( 0.5, 0.0,  0.5, 1.0));

    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;
    vec2 offs = vec2(x, y);

    vec4 pos = vertices[gl_VertexID];

    vs_out.tc = (pos.xz + offs + vec2(0.5)) / 64.0;
    vec4 position = pos + vec4(float(x - 32), 0.0,
                                               float(y - 32), 0.0);

    gl_Position = position;
}
