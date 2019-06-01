#version 440

in VS_OUT
{
    vec4 worldPos;
    vec2 texCoord;
    vec4 normal;
} vs_in;

uniform sampler2D diffuseTexture;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main()
{
    vec4 colorFromTexture = texture(diffuseTexture, vs_in.texCoord);

    WorldPosOut      = vs_in.worldPos;
    NormalOut        = vec4(vs_in.normal.xyz, 0.f);
    DiffuseOut       = vec4(0, 0, 0.2, 1);//colorFromTexture;
    MaterialSpecular = vec4(0);
}