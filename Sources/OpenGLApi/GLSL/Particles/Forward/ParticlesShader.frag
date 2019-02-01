#version 330 core

in vec2 textureCoords;

uniform sampler2D ParticleTexture;

out vec4 outputColor;

void main(void)
{
    vec4 color = texture(ParticleTexture, textureCoords);
    if ( color.a < 0.5f)
    {
        discard;
    }
    outputColor = color;
}