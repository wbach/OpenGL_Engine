#version 440
#define EPSILON 0.0002

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=6) uniform PerMaterial
{
    vec4 baseColor;
    vec4 params; // x - metallicFactor, y - roughnessFactor, z - ambientOcclusion, w - opacityCutoff
    vec4 params2; // x - normalScale,  y - useFakeLighting, z - specularStrength, w - indexOfRefraction
    vec4 hasTextures; // x - BaseColorTexture, y - NormalTexture, z -RoughnessTexture, w - MetallicTexture
    vec4 hasTextures2; // x - AmbientOcclusionTexture, y - OpacityTexture, z -DisplacementTexture, w - tiledScale
} perMaterial;

in VS_OUT
{
    vec2 texCoord;
    vec2 textureOffset;
    vec3 normal;
    float outOfViewRange;
    float visibility;
} vs_in;

layout(binding = 0) uniform sampler2D DiffuseTexture;

out vec4 outputColor;

bool Is(float v)
{
    return v > 0.5f;
}

const vec3 normalizedDummySunVector = vec3(0.5773502691896258, 0.5773502691896258, 0.5773502691896258);
const float ambientFactor = 0.1f;
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
    // vec2 textCoord = (vs_in.texCoord / perMeshObject.numberOfRows) + vs_in.textureOffset;
    vec2 textCoord = vs_in.texCoord * perMaterial.hasTextures2.w;

    if (Is(perMaterial.hasTextures.x) && Is(perApp.useTextures.x))
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
    vec4 color = colorFromTexture * perMaterial.baseColor;
    outputColor = vec4(color.rgb * dummyDiffuseFactor, color.a);


    outputColor      = vec4(outputColor.xyz, vs_in.visibility * color.a);
}
