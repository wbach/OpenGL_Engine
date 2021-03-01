#version 440

const float waveStrength = 0.02 ;
const float shineDamper  = 20.0f;
const float reflectivity = 0.6f;
const float tiledValue   = 120.f;
const float near         = 0.1f;
const float far          = 1000.0f;

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 waterColor;
    float tiledValue;
    float isSimpleRender;
    float moveFactor;
} perMeshObject;

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 toCameraVector;
    float visibility;
} vs_out;

layout(binding = 0) uniform sampler2D reflectionTexture;
layout(binding = 1) uniform sampler2D refractionTexture;
layout(binding = 2) uniform sampler2D depthMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D dudvMap;

layout (location = 0) out vec4 outputColor;

bool Is(float v)
{
    return v > .5f;
}

float calculateDistance(float depth)
{
    return 2.f * near * far / (far + near - (2.f * depth - 1.f) * (far - near));
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
    outputColor  = vec4(perMeshObject.waterColor.xyz, 0.5f);

    const vec4 fogColor = vec4(0.8, 0.8, 0.8, 1.f);
    outputColor = mix(fogColor, outputColor, vs_out.visibility);
}