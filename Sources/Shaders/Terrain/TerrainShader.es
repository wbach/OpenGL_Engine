#version 420 core

layout (quads, fractional_odd_spacing) in;

uniform sampler2D displacementTexture;

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
} tes_out;

void main(void)
{
    vec2 tc1 = mix(tes_in[0].textCoord, tes_in[1].textCoord, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].textCoord, tes_in[3].textCoord, gl_TessCoord.x);
    vec2 textCoord = mix(tc2, tc1, gl_TessCoord.y);

    vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
    p.y += texture(displacementTexture, textCoord).r * heightFactor;
    //p.y += -1.f
    vec4 P_eye = modelViewMatrix * p;

    tes_out.textCoord = textCoord;
    tes_out.world_coord = p.xyz;
    tes_out.eye_coord = P_eye.xyz;
    tes_out.position = projectionMatrix * P_eye;
    gl_Position = tes_out.position;
}
