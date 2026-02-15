#version 450
#extension GL_GOOGLE_include_directive : enable

#include "../../Common/Common.glsl"
#include "../../Common/PerApp.glsl"
#include "../../Common/PerFrameBuffer.glsl"
#include "../../Common/DefferedOut.glsl"
#include "../PerTerrainTexturesBuffer.glsl"
#include "TerrainFragShaderCommon.glsl"

struct TerrainData
{
    vec4 color;
    vec4 normal;
};

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
    float visibility;
} fs_in;


vec3 BlendNormalsRNM(vec3 n1, vec3 n2)
{
    n1 = normalize(n1);
    n2 = normalize(n2);

    vec3 t = n1 + vec3(0.0, 0.0, 1.0);
    vec3 u = n2 * vec3(-1.0, -1.0, 1.0);

    vec3 r = (t * dot(t, u) / t.z) - u;

    return normalize(r);
}

bool NormalMaping()
{
    float dist = length(perFrame.cameraPosition - fs_in.worldPos.xyz);
    return Is(perApp.useTextures.y) && (dist < perApp.viewDistance.y);
}

// vec4 textureColor(sampler2D baseTexture, sampler2D ambientOcclusionTexture, vec2 coords, vec2 useThatTexture)
// {
//     vec4 result = DEFAULT_COLOR;
//     if (Is(perApp.useTextures.x) && Is(useThatTexture.x))
//     {
//         result = texture(baseTexture, coords);
//     }

//     if (Is(perApp.useTextures.w) && Is(useThatTexture.y))
//     {
//        result.w = texture(ambientOcclusionTexture, coords).r;
//     }

//     return result;
// }

vec4 normalColor(sampler2D inputTexture, vec2 coords, float useThatTexture)
{
    if (Is(useThatTexture))
    {
        return texture(inputTexture, coords) * 2.f - 1.f;
    }
    return vec4(fs_in.normal, 1.f);
}

vec4 getTriPlanarNormal(sampler2D terrainTexture, vec3 worldCoordinates, float scale)
{
    vec3 blending = getTriPlanarTextureBlendingFactor(fs_in.normal);

    vec4 xaxis = texture(terrainTexture, worldCoordinates.yz * scale) * 2.f - 1.f;
    vec4 yaxis = texture(terrainTexture, worldCoordinates.xz * scale) * 2.f - 1.f;
    vec4 zaxis = texture(terrainTexture, worldCoordinates.xy * scale) * 2.f - 1.f;

    vec3 axisSign = sign(fs_in.normal);

    vec3 worldX = vec3(xaxis.z * axisSign.x, xaxis.y, xaxis.x);
    vec3 worldY = vec3(yaxis.x, yaxis.z * axisSign.y, yaxis.y);
    vec3 worldZ = vec3(zaxis.x, zaxis.y, zaxis.z * axisSign.z);

    return vec4(normalize(worldX * blending.x + worldY * blending.y + worldZ * blending.z), 1.0);
}

vec4 calculateBackgroundColor(float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor(fs_in.normal);

    vec4 rockTextureColor       = DEFAULT_COLOR;
    vec4 backgroundTextureColor = DEFAULT_COLOR;

    if (Is(perApp.useTextures.x) && Is(perTerrainTextures.haveTextureBackground.x))
    {
        backgroundTextureColor = getTriPlanarMappingColor(fs_in.normal, backgroundTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x);
    }

    if (Is(perApp.useTextures.w) && Is(perTerrainTextures.haveTextureBackground.z))
    {
        backgroundTextureColor.w = getTriPlanarMappingColor(fs_in.normal, backgroundTextureDisplacement, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x).r;
    }

    if (Is(perApp.useTextures.x) &&  Is(perTerrainTextures.haveTextureRock.x))
    {
        rockTextureColor = getTriPlanarMappingColor(fs_in.normal, rockTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y);
    }

    if (Is(perApp.useTextures.w) && Is(perTerrainTextures.haveTextureRock.z))
    {
        rockTextureColor.w = getTriPlanarMappingColor(fs_in.normal, rockTextureDisplacement, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y).r;
    }

    return ((backgroundTextureColor * (1.f - blendFactor)) + (rockTextureColor * blendFactor)) * backTextureAmount;
}

vec4 CalculateRGBAColor(vec2 textureCoords, vec4 blendMapColor)
{
    if (!Is(perApp.useTextures.x))
    {
        return DEFAULT_COLOR;
    }

    vec4 redTextureColor         = textureColor(redTexture, textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.x, DEFAULT_COLOR) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTexture, textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.x, DEFAULT_COLOR) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTexture, textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.x, DEFAULT_COLOR) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTexture, textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.x, DEFAULT_COLOR) * blendMapColor.a;

    return redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

float CalculateAmbientRGBAFactor(vec2 textureCoords, vec4 blendMapColor)
{
    if (!Is(perApp.useTextures.w))
    {
        return 1.f;
    }

    vec4 redTextureColor         = textureColor(redTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.z, vec4(1)) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.z, vec4(1)) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.z, vec4(1)) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.z, vec4(1)) * blendMapColor.a;

    return redTextureColor.r + greenTextureColor.r + blueTextureColor.r + alphaTextureColor.r;
}

vec4 CalculateTerrainColor(vec2 textureCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!Is(perApp.useTextures.x) && !Is(perApp.useTextures.w))
    {
        return DEFAULT_COLOR;
    }

    vec4 backgroundTextureColor = calculateBackgroundColor(backTextureAmount);
    vec4 rgbaColor = CalculateRGBAColor(textureCoords, blendMapColor);
    rgbaColor.a = CalculateAmbientRGBAFactor(textureCoords, blendMapColor);

    vec4 finalColor = backgroundTextureColor + rgbaColor;
    finalColor.a = clamp(finalColor.a, 0.0, 1.0);
    return finalColor;
}

vec4 calculateBackgroundNormal(float backTextureAmount)
{
    if (backTextureAmount < EPSILON) return vec4(0.0);

    float blendFactor = GetRockBlendFactor(fs_in.normal);

    vec3 bgNormal = fs_in.normal;
    vec3 rkNormal = fs_in.normal;

    if (Is(perTerrainTextures.haveTextureBackground.y))
    {
        bgNormal = getTriPlanarNormal(backgroundTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x).xyz;
    }

    if (Is(perTerrainTextures.haveTextureRock.y))
    {
        rkNormal = getTriPlanarNormal(rockTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y).xyz;
    }

    // Blendowanie liniowe w World Space dla triplanara jest bezpieczniejsze
    // niż RNM, jeśli oba wektory są już w World Space
    vec3 finalWorldNormal = normalize(mix(bgNormal, rkNormal, blendFactor));

    return vec4(finalWorldNormal, 1.0);
}

vec3 CalculateRGBANormalTS(vec2 uv, vec4 blendMap)
{
    vec3 normal = vec3(0,0,1);

    if(blendMap.r > 0.001)
    {
        vec3 n = normalColor(redTextureNormal, uv * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.y).xyz;
        vec3 weighted = normalize(mix(vec3(0,0,1), n, blendMap.r));

        normal = BlendNormalsRNM(normal, weighted);
    }

    if(blendMap.g > 0.001)
    {
        vec3 n = normalColor(greenTextureNormal, uv * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.y).xyz;

        vec3 weighted = normalize(mix(vec3(0,0,1), n, blendMap.g));
        normal = BlendNormalsRNM(normal, weighted);
    }

    if(blendMap.b > 0.001)
    {
        vec3 n = normalColor(blueTextureNormal, uv * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.y).xyz;

        vec3 weighted = normalize(mix(vec3(0,0,1), n, blendMap.b));

        normal = BlendNormalsRNM(normal, weighted);
    }

    if(blendMap.a > 0.001)
    {
        vec3 n = normalColor(alphaTextureNormal, uv * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.y).xyz;

        vec3 weighted = normalize(mix(vec3(0,0,1), n, blendMap.a));

        normal = BlendNormalsRNM(normal, weighted);
    }

    return normalize(normal);
}

vec4 CalculateTerrainNormal(vec2 uv, vec4 blendMap, float backgroundAmount)
{
    if (!NormalMaping())
    {
        return vec4(normalize(fs_in.normal), 1.f);
    }

    vec3 rgbaNormalTS =
        CalculateRGBANormalTS(uv, blendMap);

    vec3 rgbaNormalWS =
        normalize(fs_in.tbn * rgbaNormalTS);

    vec3 backgroundNormalWS =
        calculateBackgroundNormal(backgroundAmount).xyz;

    // BlendNormalsRNM blending in tangens space,
    vec3 finalNormal = normalize(mix(rgbaNormalWS, backgroundNormalWS, backgroundAmount));
    // vec3 finalNormal =
    //     BlendNormalsRNM(backgroundNormalWS, rgbaNormalWS);

    return vec4(finalNormal, 1.f); // w use fog
}

TerrainData GetTerrainData()
{
    vec4 blendMapColor = vec4(0.f);

    if (Is(perTerrainTextures.haveBlendMap))
    {
        blendMapColor = texture(blendMap, fs_in.texCoord);
    }

    float backTextureAmount = clamp(1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a), 0.f, 1.f);

    TerrainData result;
    result.color  = CalculateTerrainColor(fs_in.texCoord, blendMapColor, backTextureAmount);
    result.normal = CalculateTerrainNormal(fs_in.texCoord, blendMapColor, backTextureAmount);
    return result;
}

void main()
{
    TerrainData terrainData = GetTerrainData();
    const float metallic = 0.f;
    const float roughness = 0.9f;

    WorldPosOut      = fs_in.worldPos;
    DiffuseOut       = terrainData.color;
    NormalOut        = terrainData.normal;
    SurfaceParamsOut = vec4(metallic, roughness, 0.f, 0.f);
}