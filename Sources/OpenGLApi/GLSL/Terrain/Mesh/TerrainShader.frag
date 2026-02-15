#version 440 core
#extension GL_GOOGLE_include_directive : enable
#include "../PerTerrainTexturesBuffer.glsl"

const float EPSILON              = 0.00001f;
const vec4 DEFAULT_COLOR         = vec4(0.8f, 0.8f, 0.8f, 1.f);
const vec4 DEFAULT_NORMAL_COLOR  = vec4(0.5f, 0.5f, 1.0f, 1.0f); // tangent space vec4(0,0,1,1)
const vec2 thresholds            = vec2(.9f, .3f);

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

layout(binding = 0) uniform sampler2D blendMap;
// --- Background (Warstwa bazowa) ---
layout(binding = 1) uniform sampler2D backgroundTexture;
layout(binding = 2) uniform sampler2D backgroundTextureNormal;
layout(binding = 3) uniform sampler2D backgroundTextureDisplacement;

// --- Kanał Czerwony (R) ---
layout(binding = 4) uniform sampler2D redTexture;
layout(binding = 5) uniform sampler2D redTextureNormal;
layout(binding = 6) uniform sampler2D redTextureDisplacement;

// --- Kanał Zielony (G) ---
layout(binding = 7) uniform sampler2D greenTexture;
layout(binding = 8) uniform sampler2D greenTextureNormal;
layout(binding = 9) uniform sampler2D greenTextureDisplacement;

// --- Kanał Niebieski (B) ---
layout(binding = 10) uniform sampler2D blueTexture;
layout(binding = 11) uniform sampler2D blueTextureNormal;
layout(binding = 12) uniform sampler2D blueTextureDisplacement;

// --- Kanał Alfa (A) ---
layout(binding = 13) uniform sampler2D alphaTexture;
layout(binding = 14) uniform sampler2D alphaTextureNormal;
layout(binding = 15) uniform sampler2D alphaTextureDisplacement;

// --- Warstwa Skał (Slope/Rock) ---
layout(binding = 16) uniform sampler2D rockTexture;
layout(binding = 17) uniform sampler2D rockTextureNormal;
layout(binding = 18) uniform sampler2D rockTextureDisplacement;

layout(binding = 19) uniform sampler2D heightmap;
layout(binding = 20) uniform sampler2D normalmap;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SurfaceParamsOut;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
    vec4 time;
} perFrame;

vec3 CalcBumpedNormal(vec4 normalMapColor)
{
    vec3 bumpMapNormal = normalize(normalMapColor.rgb * 2.f - 1.f);
    return normalize(fs_in.tbn * bumpMapNormal);
}

vec3 BlendNormalsRNM(vec3 n1, vec3 n2)
{
    n1 = normalize(n1);
    n2 = normalize(n2);

    vec3 t = n1 + vec3(0.0, 0.0, 1.0);
    vec3 u = n2 * vec3(-1.0, -1.0, 1.0);

    vec3 r = (t * dot(t, u) / t.z) - u;

    return normalize(r);
}

bool Is(float f)
{
    return f > .5f;
}

bool NormalMaping()
{
    float dist = length(perFrame.cameraPosition - fs_in.worldPos.xyz);
    return Is(perApp.useTextures.y) && (dist < perApp.viewDistance.y);
}

vec4 textureColor(sampler2D baseTexture, sampler2D ambientOcclusionTexture, vec2 coords, vec2 useThatTexture)
{
    vec4 result = DEFAULT_COLOR;
    if (Is(perApp.useTextures.x) && Is(useThatTexture.x))
    {
        result = texture(baseTexture, coords);
    }

    if (Is(perApp.useTextures.w) && Is(useThatTexture.y))
    {
       result.w = texture(ambientOcclusionTexture, coords).r;
    }

    return result;
}

vec4 normalColor(sampler2D inputTexture, vec2 coords, float useThatTexture)
{
    if (Is(useThatTexture))
    {
        return texture(inputTexture, coords) * 2.f - 1.f;
    }
    return vec4(fs_in.normal, 1.f);
}

float GetRockBlendFactor()
{
    float value = 1.f - fs_in.normal.y;

    if (value > thresholds.x)
    {
        value = 1.f;
    }
    else if (value > thresholds.y)
    {
        value = (value - thresholds.y) / (thresholds.x - thresholds.y);
    }
    else
    {
        value = 0.f;
    }

    return value;
}

vec3 getTriPlanarTextureBlendingFactor()
{
    vec3 blending = pow(abs(fs_in.normal), vec3(4.0));
    blending /= (blending.x + blending.y + blending.z);
    return blending;

    // vec3 normal = fs_in.normal;
    // vec3 blending = abs(normal);
    // blending = normalize(max(blending, EPSILON));
    // float b = (blending.x + blending.y + blending.z);
    // blending /= vec3(b, b, b);
    // //blending.y = 0;
    // return blending;
}

vec4 getTriPlanarMappingColor(sampler2D terrainTexture, vec3 worldCoordinates, float scale)
{
    vec3 blending = getTriPlanarTextureBlendingFactor();

    vec4 xaxis = texture(terrainTexture, worldCoordinates.yz * scale);
    vec4 yaxis = texture(terrainTexture, worldCoordinates.xz * scale);
    vec4 zaxis = texture(terrainTexture, worldCoordinates.xy * scale);

    return xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
}

vec4 getTriPlanarNormal(sampler2D terrainTexture, vec3 worldCoordinates, float scale)
{
    vec3 blending = getTriPlanarTextureBlendingFactor();
    
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

    float blendFactor = GetRockBlendFactor();

    vec4 rockTextureColor       = DEFAULT_COLOR;
    vec4 backgroundTextureColor = DEFAULT_COLOR;

    if (Is(perApp.useTextures.x) && Is(perTerrainTextures.haveTextureBackground.x))
    {
        backgroundTextureColor = getTriPlanarMappingColor(backgroundTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x);
    }

    if (Is(perApp.useTextures.w) && Is(perTerrainTextures.haveTextureBackground.z))
    {
        backgroundTextureColor.w = getTriPlanarMappingColor(backgroundTextureDisplacement, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x).r;
    }

    if (Is(perApp.useTextures.x) &&  Is(perTerrainTextures.haveTextureRock.x))
    {
        rockTextureColor = getTriPlanarMappingColor(rockTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y);
    }

    if (Is(perApp.useTextures.w) && Is(perTerrainTextures.haveTextureRock.z))
    {
        rockTextureColor.w = getTriPlanarMappingColor(rockTextureDisplacement, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y).r;
    }

    return ((backgroundTextureColor * (1.f - blendFactor)) + (rockTextureColor * blendFactor)) * backTextureAmount;
}

vec4 CalculateTerrainColor(vec2 textureCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!Is(perApp.useTextures.x) && !Is(perApp.useTextures.w))
    {
        return DEFAULT_COLOR;
    }

    vec4 backgroundTextureColour = calculateBackgroundColor(backTextureAmount);
    vec4 redTextureColor         = textureColor(redTexture, redTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.xz) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTexture, greenTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.xz) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTexture, blueTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.xz) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTexture, alphaTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.xz) * blendMapColor.a;

    return backgroundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 calculateBackgroundNormal(float backTextureAmount)
{
    if (backTextureAmount < EPSILON) return vec4(0.0);

    float blendFactor = GetRockBlendFactor();
    
    vec3 bgNormal = fs_in.normal;
    vec3 rkNormal = fs_in.normal;

    if (Is(perTerrainTextures.haveTextureBackground.y)) {
        bgNormal = getTriPlanarNormal(backgroundTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x).xyz;
        // return vec4(bgNormal, 1.0);
    }

    if (Is(perTerrainTextures.haveTextureRock.y)) {
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