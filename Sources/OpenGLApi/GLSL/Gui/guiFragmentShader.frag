#version 130

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D guiTexture;
uniform vec3 color;

void main(void)
{
    vec4 tcolor = texture(guiTexture, textureCoords);
    out_Color = vec4(color, 1.0) * tcolor;
}