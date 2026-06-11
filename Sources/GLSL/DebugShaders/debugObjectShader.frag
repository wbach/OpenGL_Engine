#version 440 core

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    uint numberOfRows;
    float useTexture;
    float useNormalMap;
    float useSpecularMap;
    float shineDamper;
    float useFakeLighting;
} perMeshObject;

out vec4 outputColor;

void main()
{
    outputColor = perMeshObject.diffuse;
}
