#version 330 core

in vec4 textureCoords;
in float blendFactor;

uniform sampler2D ParticleTexture;

out vec4 outputColor;

void main(void)
{
    vec4 currentFrameColor  = texture(ParticleTexture, textureCoords.xy);
    vec4 nextFrameColor     = texture(ParticleTexture, textureCoords.zw);
    vec4 color = mix(currentFrameColor, nextFrameColor, blendFactor);

    if ( color.a < 0.5f)
    {
        discard;
    }

    outputColor = color;
}