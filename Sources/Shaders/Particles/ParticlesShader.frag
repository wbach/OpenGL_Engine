#version 330 core

in vec4 position;
in vec2 textureCoords;

uniform sampler2D ParticleTexture;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main(void)
{
	vec4 color = texture(ParticleTexture, textureCoords);
	if ( color.a < 0.5f)
    {
        discard;
    }
    WorldPosOut = position;
    DiffuseOut = vec4(0.8); DiffuseOut.w = 1.0f;
    NormalOut = vec4(0.f, 1.f, 0.f, 1.f);
    MaterialSpecular = vec4(0.f, 0.f, 0.f, 1.f);
}