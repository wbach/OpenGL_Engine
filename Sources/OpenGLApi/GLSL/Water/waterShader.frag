#version 440

const float waveStrength = 0.02 ;
const float shineDamper  = 20.0f;
const float reflectivity = 0.6f;
const float near         = 0.1f;
const float far          = 1000.0f;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 waterColor;
    float tiledValue;
    float isSimpleRender;
    float moveFactor;
} perMeshObject;

in GS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 clipSpace;
} vs_out;

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
    return 2.f * near * far / (far + near - (2.f * depth - 1.f) * (far - near));
}

float calculateEdgesFactor(float waterDepth)
{
    return clamp(waterDepth / 50.f, 0.01f, 0.09f);
}

float calculateWaterDepth(vec2 refractTexCoords)
{
    float floorDistance = calculateDistance(texture(depthMap, refractTexCoords).r);
    float waterDistance = calculateDistance(gl_FragCoord.z);
    return floorDistance - waterDistance;
}

vec2 calculateDisctortionCoords()
{
    vec2 distortedTexCoords = texture(dudvMap, vec2(vs_out.texCoord.x * perMeshObject.tiledValue + perMeshObject.moveFactor, vs_out.texCoord.y * perMeshObject.tiledValue)).rg * 0.1f;
    return vs_out.texCoord * perMeshObject.tiledValue + vec2(distortedTexCoords.x, distortedTexCoords.y+ perMeshObject.moveFactor);
}

vec3 calculateNormal(vec4 normalMapValue)
{
    vec3 normal = vec3(normalMapValue.r* 2.0f - 1.0f, normalMapValue.b * 1.0f, normalMapValue.g * 2.0f - 1.0f);
    return normalize(normal);
}

void main(void)
{
    vec2 ndc = (vs_out.clipSpace.xy/vs_out.clipSpace.w) /2.0 + 0.5 ;
    vec2 refractTexCoords = vec2(ndc.x,ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x,-ndc.y);

    MaterialSpecular = vec4(vec3(1.f), 255.f / 255.f);
    WorldPosOut      = vs_out.worldPos;
    vec2 distortedTexCoords = calculateDisctortionCoords();

    vec4 normalMapValue = texture(normalMap, distortedTexCoords);
    vec3 normal = calculateNormal(normalMapValue);

    if (Is(perMeshObject.isSimpleRender))
    {
        DiffuseOut       = vec4(perMeshObject.waterColor.xyz, 0.5f) ;//* normalMapValue;
        NormalOut        = vec4(normal, 1.f);
        return;
    }

    float waterDepth = calculateWaterDepth(refractTexCoords);
    float edgesFactor    = calculateEdgesFactor(waterDepth);
    vec2 totalDistortion =  (texture(dudvMap, distortedTexCoords).rg * 2.f - 1.f) * waveStrength * edgesFactor;

    reflectTexCoords   = reflectTexCoords + totalDistortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001f, 0.999f);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999f, -0.001f);

    refractTexCoords = refractTexCoords + totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001f, 0.999f);

    vec3 toCameraVector = normalize(perFrame.cameraPosition - vs_out.worldPos.xyz);

    float refractiveFactor  = dot(toCameraVector, vec3(0, 1, 0));
    refractiveFactor        = pow(refractiveFactor, 0.2);
    refractiveFactor        = clamp(refractiveFactor, 0.0f, 1.0f);


    float colorInte = 0.f;
    float maxDepth = 15.f;
    float scaleDepth = 2;

    if (waterDepth < 1)
    {
        colorInte = 0.f;
    }else if (waterDepth > scaleDepth*maxDepth)
    {
        colorInte = 1.f;
    }
    else
    {
        colorInte = waterDepth / maxDepth / scaleDepth;
    }

    vec4 refractColor = texture(refractionTexture, refractTexCoords);
    refractColor      = mix(refractColor, vec4(perMeshObject.waterColor.xyz, 1.f), colorInte);

    vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
    DiffuseOut        = mix(reflectColor, refractColor, refractiveFactor);
    NormalOut         = vec4(normal, 1.f);
}