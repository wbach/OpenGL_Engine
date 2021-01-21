#version 440
#define EPSILON 0.0002

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    float viewDistance;
    vec3 shadowVariables;
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
    vec3 normal;
    float outOfViewRange;
} vs_in;

uniform sampler2D DiffuseTexture;

out vec4 outputColor;

bool Is(float v)
{
    return v > 0.5f;
}

const vec3 normalizedDummySunVector = vec3(0.5773502691896258, 0.5773502691896258, 0.5773502691896258);
const float ambientFactor = 0.4f;
const vec2 defaultDiffRange = vec2(0.f, 1.f);
const vec2 diffRange = vec2(0.f, 1.f - ambientFactor);

float convertValueFromRange(float value, vec2 sourceRange, vec2 targetRange)
{
    float oldRange= sourceRange.y - sourceRange.x;
    float newRange = targetRange.y - targetRange.x;
    return (((value - sourceRange.x) * newRange) / oldRange) + targetRange.x;
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
    }

    float dummyDiffuseFactor = dot(normalizedDummySunVector, vs_in.normal);
    if (dummyDiffuseFactor > 0)
    {
        float newDiffFactor = convertValueFromRange(dummyDiffuseFactor, defaultDiffRange, diffRange);
        dummyDiffuseFactor = newDiffFactor + ambientFactor;
    }
    else
    {
        dummyDiffuseFactor = ambientFactor;
    }
    vec4 color = colorFromTexture * perMeshObject.diffuse;
    outputColor = vec4(color.rgb * dummyDiffuseFactor, color.a);
}
