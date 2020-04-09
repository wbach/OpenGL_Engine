#version 440

const float waveStrength = 0.02 ;
const float shineDamper  = 20.0f;
const float reflectivity = 0.6f;
const float tiledValue   = 120.f;
const float near         = 0.1f;
const float far          = 1000.0f;

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    float isSimpleRender;
    float moveFactor;
    vec4 waterColor;
} perMeshObject;

in VS_OUT
{
    vec2 texCoord;
    vec2 refractTexCoords;
    vec2 reflectTexCoords;
    vec3 normal;
    vec3 worldPos;
    vec3 toCameraVector;
} vs_out;

layout(binding = 0) uniform sampler2D reflectionTexture;
layout(binding = 1) uniform sampler2D refractionTexture;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 5) uniform sampler2D depthMap;
layout(binding = 4) uniform sampler2D dudvMap;

layout (location = 0) out vec3 WorldPosOut;
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

float calculateEdgesFactor()
{
    float floorDistance = calculateDistance(texture(depthMap, vs_out.refractTexCoords).r);
    float waterDistance = calculateDistance(gl_FragCoord.z);
    float waterDepth    = floorDistance - waterDistance;
    return clamp(waterDepth/2.f, 0.0f, 1.0f);
}

vec2 calculateDisctortionCoords()
{
    vec2 distortedTexCoords = texture(dudvMap, vec2(vs_out.texCoord.x * tiledValue + perMeshObject.moveFactor, vs_out.texCoord.y * tiledValue)).rg * 0.1f;
    return vs_out.texCoord * tiledValue + vec2(distortedTexCoords.x, distortedTexCoords.y+ perMeshObject.moveFactor);
}

vec3 calculateNormal(vec4 normalMapValue)
{
    vec3 normal = vec3(normalMapValue.r*2.0f - 1.0f, normalMapValue.b *1.0, normalMapValue.g*2.0f - 1.0f);
    return normalize(normal);
}

void main(void)
{
    vec2 distortedTexCoords = calculateDisctortionCoords();

    vec4 normalMapValue = texture(normalMap, distortedTexCoords);
    vec3 normal = calculateNormal(normalMapValue);

    if (Is(perMeshObject.isSimpleRender))
    {
            WorldPosOut      = vs_out.worldPos;
            DiffuseOut       = vec4(perMeshObject.waterColor.xyz, 0.5f) ;//* normalMapValue;
            NormalOut        = vec4(normal, 1.f);
            MaterialSpecular = vec4(vec3(1.f), 20.f);
            return;
    }

    float edgesFactor    = calculateEdgesFactor();
    vec2 totalDistortion =  (texture(dudvMap, distortedTexCoords).rg * 2.f - 1.f) * waveStrength * edgesFactor;

    vec2 reflectTexCoords   = vs_out.reflectTexCoords + totalDistortion;
    reflectTexCoords.x      = clamp(reflectTexCoords.x, 0.001f, 0.999f);
    reflectTexCoords.y      = clamp(reflectTexCoords.y, -0.999f, -0.001f);

    vec2 refractTexCoords   = vs_out.refractTexCoords + totalDistortion;
    refractTexCoords        = clamp(refractTexCoords, 0.001f, 0.999f);

    vec3 viewVactor         = normalize(vs_out.toCameraVector);
    float refractiveFactor  = dot(viewVactor, normal);
    refractiveFactor        = pow(refractiveFactor, 0.5);
    refractiveFactor        = clamp(refractiveFactor,0.0f,1.0f);

    vec4 reflectColor   = texture(reflectionTexture, reflectTexCoords);
    vec4 refractColor   = texture(refractionTexture, refractTexCoords);
    vec4 outColor       = mix(reflectColor, refractColor, refractiveFactor);

    WorldPosOut      = vs_out.worldPos;
    DiffuseOut       = mix(outColor, vec4(perMeshObject.waterColor.xyz, 1.f), perMeshObject.waterColor.w);
    DiffuseOut.a     = edgesFactor;
    NormalOut        = vec4(normal, 1.f);
    MaterialSpecular = vec4(vec3(1.f), 20.f);
}