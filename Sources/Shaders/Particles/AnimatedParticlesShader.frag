#version 330 core

in vec4 position;
in vec4 textureCoords;

uniform float BlendFactor;
uniform sampler2D ParticleTexture;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main(void)
{
	vec4 currentFrameColor  = texture(ParticleTexture, textureCoords.xy);
	vec4 nextFrameColor 	= texture(ParticleTexture, textureCoords.zw);
	vec4 color = mix(currentFrameColor, nextFrameColor, BlendFactor);

	if ( color.a < 0.5f)
    {
        discard;
    }
    WorldPosOut = position;
    DiffuseOut = color;//vec4(0.8); DiffuseOut.w = 1.0f;
    NormalOut = vec4(0.f, 1.f, 0.f, 0.f);
    MaterialSpecular = vec4(0.f, 0.f, 0.f, 1.f);
}