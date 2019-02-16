#version 440

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    uint numberOfRows;
    float useTexture;
    float useNormalMap;
    float shineDamper;
    float useFakeLighting;
} perMeshObject;

in VS_OUT
{
    vec4 position;
    vec2 textureCoords;
    vec3 normal;
} vs_in;

uniform sampler2D DiffuseTexture;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main(void)
{
    vec4 color = texture(DiffuseTexture, vs_in.textureCoords);
    color.xyz = color.xyz * 0.8f;
    if ( color.a < 0.5f)
    {
        discard;
    }

    WorldPosOut = vs_in.position;
    DiffuseOut = color * perMeshObject.diffuse;
    NormalOut = perMeshObject.useFakeLighting > 0.5f ? vec4( normalize(vs_in.normal), 1.f) : vec4(0.f, 1.f, 0.f, 0.f);
    MaterialSpecular = vec4(0.f, 0.f, 0.f, 1.f);
}