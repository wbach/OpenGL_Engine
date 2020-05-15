#version 440 core

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 clipPlane;
} perApp;

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
    vec2 texCoord;
    vec2 textureOffset;
} vs_in;

uniform sampler2D ModelTexture;

out vec4 outputColor;

bool Is(float v)
{
    return v > 0.5f;
}

void main(void)
{
    if (Is(perApp.useTextures.x))
    {
        vec2 textCoord = (vs_in.texCoord / perMeshObject.numberOfRows) + vs_in.textureOffset;
        float alpha = texture(ModelTexture, textCoord ).a;
        if ( alpha < 0.5f)
        {
            discard;
        }
    }
    outputColor = vec4(1.0);
}