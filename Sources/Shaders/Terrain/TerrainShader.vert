#version 420 core

out VS_OUT
{
    vec2 textCoord;
    vec3 camPos;
} vs_out;

const float SIZE = 64.f;
const float PART_SIZE = 16.0f;

const int VSCALE = 10;
const float VERTEX = 0.5f * VSCALE;
const float POSITION_OFFSET = (SIZE* VSCALE / 2.f - 0.5f) ;

uniform vec3 playerPosition;

const vec4 vertices[] = vec4[](vec4(-VERTEX, 0.0, -VERTEX, 1.0),
                               vec4( VERTEX, 0.0, -VERTEX, 1.0),
                               vec4(-VERTEX, 0.0,  VERTEX, 1.0),
                               vec4( VERTEX, 0.0,  VERTEX, 1.0));


void main(void)
{
    vs_out.camPos = playerPosition;
    
    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;
    x = x * VSCALE;
    y = y * VSCALE;

    vec2 textureOffset = vec2(x, y) + playerPosition.xz;

    vs_out.textCoord = (vertices[gl_VertexID].xz + textureOffset + vec2(0.5) ) / (SIZE * PART_SIZE);

    gl_Position = vertices[gl_VertexID] + vec4(float(x - POSITION_OFFSET), -playerPosition.y,
                                               float(y - POSITION_OFFSET), 0.0);
}
