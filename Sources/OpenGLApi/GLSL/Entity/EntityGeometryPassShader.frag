#version 440 core
const int MAX_SHADOW_MAP_CASADES = 4;
#define EPSILON 0.0002

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

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

layout (std140,binding=7) uniform ShadowsBuffer
{
    mat4 directionalLightSpace[MAX_SHADOW_MAP_CASADES];
    vec4 cascadesDistance;
    float cascadesSize;
} shadowsBuffer;

in VS_OUT
{
    vec2 textureOffset;
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
    float useShadows;
    float clipSpaceZ;
    vec4 positionInLightSpace[MAX_SHADOW_MAP_CASADES];
    float shadowMapSize;
} fs_in;

layout(binding = 0) uniform sampler2D DiffuseTexture;
layout(binding = 1) uniform sampler2D AmbientTexture;
layout(binding = 2) uniform sampler2D NormalMap;
layout(binding = 3) uniform sampler2D SpecularMap;
layout(binding = 4) uniform sampler2DShadow ShadowMap;
layout(binding = 5) uniform sampler2DShadow ShadowMap1;
layout(binding = 6) uniform sampler2DShadow ShadowMap2;
layout(binding = 7) uniform sampler2DShadow ShadowMap3;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

bool Is(float v)
{
    return v > 0.5f;
}
float CalculateShadowFactorValue(sampler2DShadow cascadeShadowMap, vec3 positionInLightSpace)
{
    // Rozmiar pojedynczego texela w UV
    float texelSize = 1.0 / fs_in.shadowMapSize;

    // Obliczenie adaptacyjnego biasu
    float distanceToCamera = length(perFrame.cameraPosition - fs_in.worldPos.xyz);
    float texelWorldSize = 1.0 / fs_in.shadowMapSize;

    // bias rośnie mocniej dla bliskiej kamery
    float bias = texelWorldSize * 1.5 + 0.005 * max(0.0, 1.0 - distanceToCamera / 75.0);

    float shadow = 0.0;
    const int filterRadius = 3; // 7x7 próbki PCF
    int sampleCount = 0;

    for (int y = -filterRadius; y <= filterRadius; y++)
    {
        for (int x = -filterRadius; x <= filterRadius; x++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            vec3 sampleCoord = vec3(positionInLightSpace.xy + offset, positionInLightSpace.z - bias);
            shadow += texture(cascadeShadowMap, sampleCoord);
            sampleCount++;
        }
    }

    shadow /= float(sampleCount); // średnia → miękkie cienie

    // Ogólna jasność cienia, zachowujemy trochę „podświetlenia”
    float finalShadow = clamp(0.7 + shadow, 0.0, 1.0);
    return finalShadow;
}

float CalculateShadowFactor()
{
    if (!Is(perApp.shadowVariables.x) || shadowsBuffer.cascadesSize == 0)
        return 1.f;

    float z = fs_in.clipSpaceZ;

    // Kaskada 1
    if (shadowsBuffer.cascadesSize >= 1 && z < shadowsBuffer.cascadesDistance.x)
    {
        return CalculateShadowFactorValue(ShadowMap, fs_in.positionInLightSpace[0].xyz);
    }
    // Kaskada 2 z płynnym przejściem
    else if (shadowsBuffer.cascadesSize >= 2 && z < shadowsBuffer.cascadesDistance.y)
    {
        float blendRange = 2.0 / fs_in.shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.x - blendRange,
                                  shadowsBuffer.cascadesDistance.x, z);

        float shadowPrev = CalculateShadowFactorValue(ShadowMap, fs_in.positionInLightSpace[0].xyz);
        float shadowCurr = CalculateShadowFactorValue(ShadowMap1, fs_in.positionInLightSpace[1].xyz);
        return mix(shadowPrev, shadowCurr, factor);
    }
    // Kaskada 3
    else if (shadowsBuffer.cascadesSize >= 3 && z < shadowsBuffer.cascadesDistance.z)
    {
        float blendRange = 2.0 / fs_in.shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.y - blendRange,
                                  shadowsBuffer.cascadesDistance.y, z);

        float shadowPrev = CalculateShadowFactorValue(ShadowMap1, fs_in.positionInLightSpace[1].xyz);
        float shadowCurr = CalculateShadowFactorValue(ShadowMap2, fs_in.positionInLightSpace[2].xyz);
        return mix(shadowPrev, shadowCurr, factor);
    }
    // Kaskada 4
    else if (shadowsBuffer.cascadesSize >= 4 && z < shadowsBuffer.cascadesDistance.w)
    {
        float blendRange = 2.0 / fs_in.shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.z - blendRange,
                                  shadowsBuffer.cascadesDistance.z, z);

        float shadowPrev = CalculateShadowFactorValue(ShadowMap2, fs_in.positionInLightSpace[2].xyz);
        float shadowCurr = CalculateShadowFactorValue(ShadowMap3, fs_in.positionInLightSpace[3].xyz);
        return mix(shadowPrev, shadowCurr, factor);
    }

    return 1.f;
}

vec4 CalcBumpedNormal(vec2 text_coords)
{
    vec3 bumpMapNormal = texture(NormalMap, text_coords).xyz;
    bumpMapNormal = bumpMapNormal * 2.f - 1.f;
    return vec4(normalize(fs_in.tbn * bumpMapNormal) , 1.f);
}

bool NormalMaping()
{
    float dist = length(perFrame.cameraPosition - fs_in.worldPos.xyz);
    return Is(perApp.useTextures.y) && (dist < perApp.viewDistance.y);
}

vec4 GetNormal(vec2 textCoord)
{
    if (Is(perMeshObject.useFakeLighting))
    {
        return vec4(0.0f, 1.0f, 0.0f, 1.f); // w use fog
    }
    else
    {
        return (Is(perMeshObject.haveNormalMap) && NormalMaping()) ? CalcBumpedNormal(textCoord) : vec4(normalize(fs_in.normal), 1.f);
    }
}

vec4 GetSpecular(vec2 textCoord)
{
    if (Is(perMeshObject.haveSpecularMap) && Is(perApp.useTextures.z))
    {
        return vec4((texture(SpecularMap, textCoord) * perMeshObject.specular).xyz , perMeshObject.shineDamper);

    }
    else
    {
       return vec4(perMeshObject.specular.xyz, perMeshObject.shineDamper / 255.f);
    }
}

void main()
{
    vec4 colorFromTexture = vec4(1.f, 1.f, 1.f, 1.f);
    vec2 textCoord = (fs_in.texCoord / perMeshObject.numberOfRows) + fs_in.textureOffset;

    if (Is(perMeshObject.haveDiffTexture) && Is(perApp.useTextures.x))
    {
        colorFromTexture = texture(DiffuseTexture, textCoord);
        //if(colorFromTexture.a < 0.485f)
        if(colorFromTexture.a < 0.1f)
        {
            discard;
        }
        else
        {
            colorFromTexture.a = 1.f;
        }
    }

    WorldPosOut      = fs_in.worldPos;
    NormalOut        = GetNormal(textCoord);
    DiffuseOut       = colorFromTexture * perMeshObject.diffuse;
    DiffuseOut       = vec4(DiffuseOut.rgb * CalculateShadowFactor(), DiffuseOut.a);
    MaterialSpecular = GetSpecular(textCoord);


    // if (shadowsBuffer.cascadesSize > 0)
    // {
    //     if (fs_in.clipSpaceZ < shadowsBuffer.cascadesDistance.x && shadowsBuffer.cascadesSize >= 1)
    //     {
    //         DiffuseOut = vec4(1, 0, 0, 1);
    //     }
    //     else if (fs_in.clipSpaceZ < shadowsBuffer.cascadesDistance.y && shadowsBuffer.cascadesSize >= 2 )
    //     {
    //         DiffuseOut = vec4(1, 1, 0, 1);
    //     }
    //     else if (fs_in.clipSpaceZ < shadowsBuffer.cascadesDistance.z && shadowsBuffer.cascadesSize >= 3)
    //     {
    //         DiffuseOut = vec4(0, 1, 0, 1);
    //     }
    //     else if (fs_in.clipSpaceZ < shadowsBuffer.cascadesDistance.w && shadowsBuffer.cascadesSize >= 4)
    //     {
    //         DiffuseOut = vec4(0, 0, 1, 1);
    //     }
    //     else
    //     {
    //         DiffuseOut = vec4(1, 0, 1, 1);
    //     }
    // }
    // else
    // {
    //     DiffuseOut = vec4(0, 1, 1, 1);
    // }
}
