#version 450 core

layout(location = 0) out vec4 WorldPosOut;
layout(location = 1) out vec4 ColorMapOut;
layout(location = 2) out vec4 NormalOut;
layout(location = 3) out vec4 SurfaceParamsOut;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D diffuseTexture;

void main()
{
    WorldPosOut = vec4(FragPos, 1.0);

    ColorMapOut = texture(diffuseTexture, TexCoord);
    NormalOut = vec4(normalize(Normal), 1.0);

    float roughness = 0.8;
    float metallic = 0.0;
    SurfaceParamsOut = vec4(roughness, metallic, 0.0, 1.0);
}
