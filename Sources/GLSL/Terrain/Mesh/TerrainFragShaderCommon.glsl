const float EPSILON              = 0.00001f;
const vec4 DEFAULT_COLOR         = vec4(0.8f, 0.8f, 0.8f, 1.f);
const vec4 DEFAULT_NORMAL_COLOR  = vec4(0.5f, 0.5f, 1.0f, 1.0f); // tangent space vec4(0,0,1,1)
const vec2 thresholds            = vec2(.9f, .3f);

layout(binding = 0) uniform sampler2D blendMap;

layout(binding = 1) uniform sampler2D backgroundTexture;
layout(binding = 2) uniform sampler2D backgroundTextureNormal;
layout(binding = 3) uniform sampler2D backgroundTextureDisplacement;

layout(binding = 4) uniform sampler2D redTexture;
layout(binding = 5) uniform sampler2D redTextureNormal;
layout(binding = 6) uniform sampler2D redTextureDisplacement;

layout(binding = 7) uniform sampler2D greenTexture;
layout(binding = 8) uniform sampler2D greenTextureNormal;
layout(binding = 9) uniform sampler2D greenTextureDisplacement;

layout(binding = 10) uniform sampler2D blueTexture;
layout(binding = 11) uniform sampler2D blueTextureNormal;
layout(binding = 12) uniform sampler2D blueTextureDisplacement;

layout(binding = 13) uniform sampler2D alphaTexture;
layout(binding = 14) uniform sampler2D alphaTextureNormal;
layout(binding = 15) uniform sampler2D alphaTextureDisplacement;

layout(binding = 16) uniform sampler2D rockTexture;
layout(binding = 17) uniform sampler2D rockTextureNormal;
layout(binding = 18) uniform sampler2D rockTextureDisplacement;

layout(binding = 19) uniform sampler2D heightmap;
layout(binding = 20) uniform sampler2D normalmap;

float GetRockBlendFactor(vec3 normal)
{
    float value = 1.f - normal.y;

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

vec3 getTriPlanarTextureBlendingFactor(vec3 normal)
{
    vec3 blending = pow(abs(normal), vec3(4.0));
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

vec4 getTriPlanarMappingColor(vec3 normal, sampler2D terrainTexture, vec3 worldCoordinates, float scale)
{
    vec3 blending = getTriPlanarTextureBlendingFactor(normal);

    vec4 xaxis = texture(terrainTexture, worldCoordinates.yz * scale);
    vec4 yaxis = texture(terrainTexture, worldCoordinates.xz * scale);
    vec4 zaxis = texture(terrainTexture, worldCoordinates.xy * scale);

    return xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
}

vec4 textureColor(sampler2D inputTexture, vec2 coords, float useThatTexture, vec4 defaultColor)
{
    if (Is(useThatTexture))
    {
        return texture(inputTexture, coords);
    }

    return defaultColor;
}
