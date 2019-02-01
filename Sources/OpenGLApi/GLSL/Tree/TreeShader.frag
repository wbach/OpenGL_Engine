#version 330 core

struct SMaterial
{
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shineDamper;
};

in vec4 position;
in vec2 textureCoords;
in vec3 normal;

uniform SMaterial ModelMaterial;
uniform sampler2D DiffuseTexture;
uniform float UseShading;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main(void)
{
    vec4 color = texture(DiffuseTexture, textureCoords);
    color.xyz = color.xyz * 0.8f;
    if ( color.a < 0.5f)
    {
        discard;
    }
    WorldPosOut = position;
    DiffuseOut = color * vec4(ModelMaterial.diffuse, 1.0f);//vec4(vec3(0.8), 1.f);
    NormalOut = UseShading > 0.5f ? vec4( normalize(normal), 1.f) : vec4(0.f, 1.f, 0.f, 0.f);
    MaterialSpecular = vec4(0.f, 0.f, 0.f, 1.f);
}