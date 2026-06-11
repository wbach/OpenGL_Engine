#version 440
#extension GL_GOOGLE_include_directive : enable
#include "../Common/PerFrameBuffer.glsl"

const float waveStrength = 0.02 ;
const float shineDamper  = 20.0f;
const float reflectivity = 0.6f;

layout (std140, align=16, binding=8) uniform WaterTileMeshBuffer
{
    vec4 waterColor;
    vec4 tilePosAndScale;
    vec4 params; // x - deltaTime, y - waveSpeed, z - tiledValue, w - isSimpleRender
    vec4 waveParams;
    vec4 projParams;
    vec4 waterDepthVisibility; // x - maxVisibleDepth, y = scaleDepth z - waterColorBlendFactor, w - softEdgeDistance
} waterTileMeshBuffer;

in GS_OUT
{
    vec4 worldPos;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec4 clipSpace;
} gs_out;

layout(binding = 0) uniform sampler2D reflectionTexture;
layout(binding = 1) uniform sampler2D refractionTexture;
layout(binding = 2) uniform sampler2D depthMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D dudvMap;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

bool Is(float v)
{
    return v > .5f;
}

float calculateDistance(float depth)
{
    const float near         = waterTileMeshBuffer.projParams.x;
    const float far          = waterTileMeshBuffer.projParams.y;
    return 2.f * near * far / (far + near - (2.f * depth - 1.f) * (far - near));
}

float calculateEdgesFactor(float waterDepth)
{
    const float SOFT_EDGE_DEPTH = waterTileMeshBuffer.waterDepthVisibility.w;
    float factor = clamp(waterDepth / SOFT_EDGE_DEPTH, 0.0, 1.0);
    return factor;
}

float calculateWaterDepth(vec2 refractTexCoords)
{
    float floorDistance = calculateDistance(texture(depthMap, refractTexCoords).r);
    float waterDistance = calculateDistance(gl_FragCoord.z);
    return floorDistance - waterDistance;
}

vec2 calculateDisctortionCoords()
{
    float tiledValue = waterTileMeshBuffer.params.z;
    float moveFactor = waterTileMeshBuffer.params.x;
    float waveSpeed = waterTileMeshBuffer.params.y;

    vec2 distortedTexCoords = texture(dudvMap, vec2(gs_out.texCoord.x * tiledValue + moveFactor, gs_out.texCoord.y * tiledValue)).rg * 0.1f;
    return gs_out.texCoord * tiledValue + vec2(distortedTexCoords.x, distortedTexCoords.y+ moveFactor);
}

vec3 decodeNormal(vec4 normalMapValue)
{
    vec3 n;
    n.xy = normalMapValue.rg * 2.0 - 1.0;
    n.z  = normalMapValue.b * 2.0 - 1.0; // jeśli normal mapa ma blue w formacie 0..1 → 0..1
    return normalize(n);
}

mat3 getTBN()
{
    vec3 T = normalize(gs_out.tangent);
    vec3 B = normalize(gs_out.bitangent);
    vec3 N = normalize(gs_out.normal);

    T = normalize(T - dot(T, N) * N);
    B = normalize(cross(N, T));

    return mat3(T, B, N);
}

vec3 calculateWorldNormal(vec4 normalMapValue)
{
    vec3 n = decodeNormal(normalMapValue); // tangent space normal
    mat3 TBN = getTBN();                   // world-space basis
    return normalize(TBN * n);             // world-space normal
}

float CalculateRefractionWithWaterColorBlendFactor(float waterDepth)
{
    float colorInte = 0.f;
    // float maxDepth = 15.f;
    // float scaleDepth = 2;
    float maxDepth = waterTileMeshBuffer.waterDepthVisibility.x;
    float scaleDepth = waterTileMeshBuffer.waterDepthVisibility.y;

    if (waterDepth < 1)
    {
        colorInte = 0.f;
    }
    else if (waterDepth > scaleDepth * maxDepth)
    {
        colorInte = 1.f;
    }
    else
    {
        colorInte = waterDepth / maxDepth / scaleDepth;
    }

    return max(waterTileMeshBuffer.waterDepthVisibility.z, colorInte);
}

float dither4x4(vec2 position)
{
    int x = int(mod(position.x, 4.0));
    int y = int(mod(position.y, 4.0));
    int index = x + y * 4;

    float bayer[16] = float[](
        0.0/16.0, 8.0/16.0, 2.0/16.0, 10.0/16.0,
        12.0/16.0, 4.0/16.0, 14.0/16.0, 6.0/16.0,
        3.0/16.0, 11.0/16.0, 1.0/16.0, 9.0/16.0,
        15.0/16.0, 7.0/16.0, 13.0/16.0, 5.0/16.0
    );

    return bayer[index];
}

float dither8x8(vec2 position)
{
    int x = int(mod(position.x, 8.0));
    int y = int(mod(position.y, 8.0));

    // Matryca 8x8 (64 wartości)
    const int bayer8[64] = int[](
         0, 32,  8, 40,  2, 34, 10, 42,
        48, 16, 56, 24, 50, 18, 58, 26,
        12, 44,  4, 36, 14, 46,  6, 38,
        60, 28, 52, 20, 62, 30, 54, 22,
         3, 35, 11, 43,  1, 33,  9, 41,
        51, 19, 59, 27, 49, 17, 57, 25,
        15, 47,  7, 39, 13, 45,  5, 37,
        63, 31, 55, 23, 61, 29, 53, 21
    );

    return float(bayer8[x + y * 8]) / 64.0;
}

void main(void)
{
    MaterialSpecular = vec4(vec3(1.f), 255.f / 255.f);
    WorldPosOut      = gs_out.worldPos;
    vec2 distortedTexCoords = calculateDisctortionCoords();

    vec4 normalMapValue = texture(normalMap, distortedTexCoords);
    //vec3 normal = gs_out.normal;//calculateNormal(normalMapValue);
    vec3 normal = calculateWorldNormal(normalMapValue);
    vec3 normalGS = normalize(gs_out.normal);
    const float normalStrength =  waterTileMeshBuffer.waveParams.w;
    normal = normalize(mix(normalGS, normal, normalStrength));

    DiffuseOut       = vec4(waterTileMeshBuffer.waterColor.xyz, 1.0f);
    NormalOut        = vec4(normal, 1.f);

    float isSimpleRender = waterTileMeshBuffer.params.w;
    if (Is(isSimpleRender))
    {
        DiffuseOut       = vec4(waterTileMeshBuffer.waterColor.xyz, 0.5f);
        return;
    }

    vec2 ndc = (gs_out.clipSpace.xy / gs_out.clipSpace.w) / 2.0f + 0.5f ;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

    float waterDepth = calculateWaterDepth(refractTexCoords);
    if (waterDepth < 0.0)
    {
        waterDepth = calculateWaterDepth(distortedTexCoords);
    }

    float edgesFactor    = calculateEdgesFactor(waterDepth);
    float ditherThreshold = dither8x8(gl_FragCoord.xy);
    if (edgesFactor < ditherThreshold)
    {
        discard;
    }

    vec2 totalDistortion = vec2(0);//  (texture(dudvMap, distortedTexCoords).rg * 2.f - 1.f) * waveStrength * edgesFactor;

    reflectTexCoords   = reflectTexCoords + totalDistortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001f, 0.999f);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999f, -0.001f);

    refractTexCoords = refractTexCoords + totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001f, 0.999f);

    vec3 toCameraVector = normalize(perFrame.cameraPosition - gs_out.worldPos.xyz);
    float cosTheta = dot(toCameraVector, normal);
    const float R0 = 0.02;
    float fresnel = R0 + (1.0 - R0) * pow(1.0 - max(cosTheta, 0.0), 4.0);

    vec4 refractColor = texture(refractionTexture, refractTexCoords);
    vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);

    // const float far = waterTileMeshBuffer.projParams.y;
    // bool isUnderwater = waterDepth / far < 0.5f;
    // if (!isUnderwater)
    // {
    //    refractColor = vec4(0, 0 ,0, 1);
    // }

    refractColor    = mix(refractColor, vec4(waterTileMeshBuffer.waterColor.xyz, 1.f), CalculateRefractionWithWaterColorBlendFactor(waterDepth));
    DiffuseOut      = mix(refractColor, reflectColor, fresnel);
    DiffuseOut.a    = 1.0;
}
