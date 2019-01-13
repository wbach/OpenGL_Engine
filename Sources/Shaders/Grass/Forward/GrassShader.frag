#version 330
#define EPSILON 0.001

uniform sampler2D Texture0;

in vec2 TexCoord;
in vec3 WorldPos;

out vec4 outputColor;

void main()
{   
    vec4 texture_color  = texture(Texture0, TexCoord);
    if(texture_color.a < .5f) discard;
    outputColor       = texture_color;
}