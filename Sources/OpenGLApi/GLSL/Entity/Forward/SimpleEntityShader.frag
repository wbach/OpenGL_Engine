#version 440
#define EPSILON 0.0002

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    float viewDistance;
    vec3 shadowVariables;
    vec4 clipPlane;
} perApp;

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    uint numberOfRows;
    float haveDiffTexture;
    float haveNormalMap;
    float haveSpecularMap;
    float shineDamper;
    float useFakeLighting;
} perMeshObject;

in VS_OUT
{
    vec2 texCoord;
    vec2 textureOffset;
    float outOfViewRange;
} vs_in;

uniform sampler2D DiffuseTexture;

out vec4 outputColor;

bool Is(float v)
{
    return v > 0.5f;
}

void main()
{
    if (Is(vs_in.outOfViewRange))
        discard;

    vec4 colorFromTexture = vec4(1.f, 1.f, 1.f, 1.f);
    vec2 textCoord = (vs_in.texCoord / perMeshObject.numberOfRows) + vs_in.textureOffset;

    if (Is(perMeshObject.haveDiffTexture) && Is(perApp.useTextures.x))
    {
        colorFromTexture = texture(DiffuseTexture, textCoord);
        if(!Is(colorFromTexture.a))
        {
            discard;
        }
    }

    outputColor = colorFromTexture * perMeshObject.diffuse;
}
