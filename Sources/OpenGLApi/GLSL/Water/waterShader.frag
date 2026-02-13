#version 440

const float waveStrength = 0.02 ;
const float shineDamper  = 20.0f;
const float reflectivity = 0.6f;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
    vec4 time;
} perFrame;

layout (std140, align=16, binding=8) uniform WaterTileMeshBuffer
{
    vec4 waterColor;
    vec4 tilePosAndScale;
    vec4 params; // x - deltaTime, y - waveSpeed, z - tiledValue, w - isSimpleRender
    vec4 waveParams;
    vec4 projParams;
    vec4 waterDepthVisibility; // x - maxVisibleDepth, y = scaleDepth
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


const float EDGE_DEPTH_SCALE = 50.0; 
const float EDGE_MIN_FACTOR  = 0.01;  
const float EDGE_MAX_FACTOR  = 0.09;  
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
    float factor = waterDepth / EDGE_DEPTH_SCALE;
    return clamp(factor, EDGE_MIN_FACTOR, EDGE_MAX_FACTOR);
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

    // Gram-Schmidt orthonormalization — poprawia błędy numeryczne
    T = normalize(T - dot(T, N) * N);
    B = normalize(cross(N, T)); // zawsze prostopadły

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

void main(void)
{
    MaterialSpecular = vec4(vec3(1.f), 255.f / 255.f);
    WorldPosOut      = gs_out.worldPos;
    vec2 distortedTexCoords = calculateDisctortionCoords();

    vec4 normalMapValue = texture(normalMap, distortedTexCoords);
    //vec3 normal = gs_out.normal;//calculateNormal(normalMapValue);
    vec3 normal = calculateWorldNormal(normalMapValue);
    vec3 normalGS = normalize(gs_out.normal);  

    DiffuseOut       = vec4(waterTileMeshBuffer.waterColor.xyz, 1.0f);
    NormalOut        = vec4(normal, 1.f);


    float isSimpleRender = waterTileMeshBuffer.params.w;
    if (Is(isSimpleRender))
    {
       //DiffuseOut       = vec4(waterTileMeshBuffer.waterColor.xyz, 0.5f);
        return;
    }

    vec2 ndc = (gs_out.clipSpace.xy / gs_out.clipSpace.w) / 2.0f + 0.5f ;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

    float waterDepth = calculateWaterDepth(refractTexCoords);
    float edgesFactor    = calculateEdgesFactor(waterDepth);
    vec2 totalDistortion =  (texture(dudvMap, distortedTexCoords).rg * 2.f - 1.f) * waveStrength * edgesFactor;

    reflectTexCoords   = reflectTexCoords + totalDistortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001f, 0.999f);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999f, -0.001f);

    refractTexCoords = refractTexCoords + totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001f, 0.999f);

    vec3 toCameraVector = normalize(perFrame.cameraPosition - gs_out.worldPos.xyz);

    float refractiveFactor  = dot(toCameraVector, gs_out.normal);
    refractiveFactor        = pow(refractiveFactor, 0.2);
    refractiveFactor        = clamp(refractiveFactor, 0.0f, 1.0f);

    vec4 refractColor = texture(refractionTexture, refractTexCoords);
    vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);

    const float far          = waterTileMeshBuffer.projParams.y;
    float depth = calculateWaterDepth(refractTexCoords) / far;
    bool isUnderwater = depth < 0.5f;
    if (!isUnderwater)
    {
       refractColor = vec4(0, 0 ,0, 1);
    }
    DiffuseOut.a = 1.f;

    refractColor    = mix(refractColor, vec4(waterTileMeshBuffer.waterColor.xyz, 1.f), CalculateRefractionWithWaterColorBlendFactor(waterDepth));
    DiffuseOut      = mix(reflectColor, refractColor, refractiveFactor);
    DiffuseOut.a    = 1.0;
}
