#version 420 core

out VS_OUT
{
    vec2 textCoord;
} vs_out;

const float SCALE = 10.f;  // have to the same as in c++ code, TerrainDef.h
const float VERTEX = 0.5f * SCALE;
const float POSITION_OFFSET = 31.5f * SCALE;
const float INVERTED_SCALE = 1.f / SCALE;

const vec4 vertices[] = vec4[](vec4(-VERTEX, 0.0, -VERTEX, 1.0),
                               vec4( VERTEX, 0.0, -VERTEX, 1.0),
                               vec4(-VERTEX, 0.0,  VERTEX, 1.0),
                               vec4( VERTEX, 0.0,  VERTEX, 1.0));


void main(void)
{
    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;
    vec2 textureOffset = vec2(x, y);

    vs_out.textCoord = (vertices[gl_VertexID].xz * INVERTED_SCALE + textureOffset + vec2(0.5) ) / 64.0;

    gl_Position = vertices[gl_VertexID] + vec4(float(x*SCALE - POSITION_OFFSET), 0.0,
                                               float(y*SCALE - POSITION_OFFSET), 0.0);
}
