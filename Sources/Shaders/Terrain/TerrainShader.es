#version 420 core

layout (quads, fractional_odd_spacing) in;

uniform sampler2D displacementMap;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform float heightFactor;

in TCS_OUT
{
    vec2 textCoord;
} tes_in[];

out TES_OUT
{
    vec2 textCoord;
    vec3 world_coord;
    vec3 eye_coord;
    vec4 position;
    vec3 normal;
    float height;
} tes_out;

float GetHeight(vec2 v)
{
    return texture(displacementMap, v).r * heightFactor;
}

const float resolution = 1.f / 4096.f;

void main(void)
{
    vec2 tc1 = mix(tes_in[0].textCoord, tes_in[1].textCoord, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].textCoord, tes_in[3].textCoord, gl_TessCoord.x);
    vec2 textCoord = mix(tc2, tc1, gl_TessCoord.y);

    vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
    p.y += GetHeight(textCoord);

    tes_out.height = GetHeight(textCoord);

    float heightL = GetHeight(textCoord - vec2(resolution, 0));
    float heightR = GetHeight(textCoord + vec2(resolution, 0));
    float heightD = GetHeight(textCoord - vec2(0, resolution));
    float heightU = GetHeight(textCoord + vec2(0, resolution));

    tes_out.normal = normalize( vec3(heightL - heightR, 2.0f, heightD - heightU) );

    vec4 P_eye = modelViewMatrix * p;

    tes_out.textCoord = textCoord;
    tes_out.world_coord = p.xyz;
    tes_out.eye_coord = P_eye.xyz;
    tes_out.position = projectionMatrix * P_eye;
    gl_Position = tes_out.position;
}
