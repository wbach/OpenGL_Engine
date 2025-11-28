#version 440
#define M_PI    3.14159265358979323846264338327950288   /* pi */

// sync with CPU
#define MAX_DIR_LIGHTS   2
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS  32
const int MAX_SHADOW_MAP_CASADES = 4;

struct SMaterial
{
    vec3 ambient_;
    vec3 diffuse_;
    vec3 specular_;
    float shineDamper_;
};

struct Light
{
    vec3  position_;
    vec3  color_;
    vec3  attenuation_;
    float cutOff_;
    int   type_;
    vec3  direction_;
};

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables; // x - useShadows, y - unused, z - shadowMapSize, w - unused
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection; // x - near, y -far
} perFrame;

layout (std140, align=16, binding=6) uniform LightPass
{
    vec4 skyColor;
    vec2 screenSize;
    float viewDistance;
    uvec3 numberOfLights;
} lightsPass;

layout (std140,binding=7) uniform ShadowsBuffer
{
    mat4 directionalLightSpace[MAX_SHADOW_MAP_CASADES];
    vec4 cascadesDistance;
    float cascadesSize;
} shadowsBuffer;

layout(std140, binding = 9) uniform DirectionalLights
{
    vec4 direction[MAX_DIR_LIGHTS]; // xyz = direction, w unused
    vec4 color[MAX_DIR_LIGHTS];     // rgb, a = intensity
} dirLights;

layout(std140, binding = 10) uniform PointLights
{
    vec4 color[MAX_POINT_LIGHTS];   // rgb, a = intensity
    vec4 params[MAX_POINT_LIGHTS];  // x = intensity (redundant), y = range, z = falloffExponent, w unused
    vec4 position[MAX_POINT_LIGHTS]; // xyz = world position, w unused
} pointLights;

layout(std140, binding = 11) uniform SpotLights
{
    vec4 color[MAX_SPOT_LIGHTS];    // rgb, a = intensity
    vec4 params[MAX_SPOT_LIGHTS];   // x = intensity (redundant), y = range, z = falloffExponent, w unused
    vec4 direction[MAX_SPOT_LIGHTS]; // xyz = spot direction (unit), w unused
    vec4 cutoff[MAX_SPOT_LIGHTS];     // x = innerCutoff(cos), y = outerCutoff(cos), z/w unused
    vec4 position[MAX_SPOT_LIGHTS];  // xyz = world position of spot, w unused
} spotLights;

layout(binding = 0) uniform sampler2D PositionMap;
layout(binding = 1) uniform sampler2D ColorMap;
layout(binding = 2) uniform sampler2D NormalMap;
layout(binding = 3) uniform sampler2D SpecularMap;
layout(binding = 4) uniform sampler2D DepthTexture;
layout(binding = 5) uniform sampler2D SkyTexture;
layout(binding = 6) uniform sampler2DShadow shadowMap0;
layout(binding = 7) uniform sampler2DShadow shadowMap1;
layout(binding = 8) uniform sampler2DShadow shadowMap2;
layout(binding = 9) uniform sampler2DShadow shadowMap3;

in VS_OUT
{
    vec2 textureCoords;
    vec3 cameraPosition;
} vs_in;

out vec4 FragColor;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / lightsPass.screenSize;
}

float max3(vec3 v)
{
  return max(max (v.x, v.y), v.z);
}

bool Is(float v)
{
    return v > 0.5f;
}

vec4 CalculateBaseLight(SMaterial material, vec3 lightDirection, vec3 worldPosition, vec3 unitNormal, vec3 lightColor)
{
    // light_direction is the vector from fragment TO light (L)
    float NdotL = max(dot(unitNormal, lightDirection), 0.0);

    vec3 ambient_color   = material.ambient_;
    vec3 diffuse_color   = lightColor * NdotL * material.diffuse_;
    vec3 specular_color  = vec3(0.0);

    if (length(material.specular_) > 0.001)
    {
        // view direction: from fragment TO camera
        vec3 viewDir = normalize(perFrame.cameraPosition - worldPosition);
        // Blinn-Phong halfway vector
        vec3 H = normalize(viewDir + lightDirection);
        float NdotH = max(dot(unitNormal, H), 0.0);

        float specFactor = pow(NdotH, max(material.shineDamper_, 1.0));
        specular_color = lightColor * material.specular_ * specFactor;
    }

    return vec4(ambient_color + diffuse_color + specular_color, 1.0);
}

vec4 CalculatePointLight(SMaterial material, int idx, vec3 worldPosition, vec3 unitNormal)
{
    vec3 lightPos = pointLights.position[idx].xyz;
    vec3 toLightVector    = lightPos - worldPosition;
    float distanceToLight = length(toLightVector);
    vec3 lightDirection    = normalize(toLightVector);

    vec3 rgb = pointLights.color[idx].rgb;
    float intensity = pointLights.color[idx].a;
    if (intensity <= 0.0) intensity = pointLights.params[idx].x; // fallback
    vec3 lightColor = rgb * intensity;

    vec4 color = CalculateBaseLight(material, lightDirection, worldPosition, unitNormal, lightColor);

    float range = pointLights.params[idx].y;
    float falloffExp = max(pointLights.params[idx].z, 0.0001);

    float x = clamp(1.0 - distanceToLight / max(range, 0.0001), 0.0, 1.0);
    float att = pow(x, falloffExp);
    return color * att;
}

vec4 CalcSpotLight(SMaterial material, int idx, vec3 worldPosition, vec3 unitNormal)
{
    vec3 lightPos = spotLights.position[idx].xyz;
    vec3 toLightVector = lightPos - worldPosition;
    float distanceToLight = length(toLightVector);
    vec3 L = normalize(toLightVector);

    vec3 spotDir = normalize(spotLights.direction[idx].xyz);
    float cosTheta = dot(-L, spotDir);

    float innerCut = spotLights.cutoff[idx].x; // cos(inner)
    float outerCut = spotLights.cutoff[idx].y; // cos(outer)

    if (cosTheta <= outerCut)
        return vec4(0.0, 0.0, 0.0, 1.0);

    vec3 rgb = spotLights.color[idx].rgb;
    float intensity = spotLights.color[idx].a;
    if (intensity <= 0.0) intensity = spotLights.params[idx].x; // fallback

    vec3 light_color = rgb * intensity;

    // calculate smooth angular falloff between inner and outer
    float angularFactor = 1.0;
    if (cosTheta < innerCut)
    {
        float t = (cosTheta - outerCut) / max((innerCut - outerCut), 1e-5);
        angularFactor = clamp(t, 0.0, 1.0);
        // optionally apply smoothstep for softer edge
        angularFactor = smoothstep(0.0, 1.0, angularFactor);
    }

    // distance falloff same as point
    float range = spotLights.params[idx].y;
    float falloffExp = max(spotLights.params[idx].z, 0.0001);
    float x = clamp(1.0 - distanceToLight / max(range, 0.0001), 0.0, 1.0);
    float att = pow(x, falloffExp);

    vec4 color = CalculateBaseLight(material, L, worldPosition, unitNormal, light_color);

    return color * att * angularFactor;
}

float CalculateShadowFactorValue(sampler2DShadow cascadeShadowMap, vec4 positionInLightSpace)
{
    float shadowMapSize  = perApp.shadowVariables.z;
    float texelSize = 1.f / shadowMapSize;

    positionInLightSpace /= positionInLightSpace.w;

    float factor = 0.0;

    float a = 0;
    for (int y = -1 ; y <= 1 ; y++)
    {
        for (int x = -1 ; x <= 1 ; x++)
        {
            vec2 offsets = vec2(float(x) * texelSize, float(y) * texelSize);
            vec3 uvc = vec3(positionInLightSpace.xy + offsets, positionInLightSpace.z);

            if (texture(cascadeShadowMap, uvc) >  0.f)
            {
                //factor += (fs_in.shadowTransition * 0.4f);
                factor += 1.f;
            }
           a++;
        }
    }
    float value = (.5f + (factor / a));
    if( value > 1.f )
        value = 1.f ;

    return value ;
}

vec4 getPositionInLightSpace(int index, vec3 worldPosition)
{
    return shadowsBuffer.directionalLightSpace[index] * vec4(worldPosition, 1.0);
}

float CalculateShadowFactor(vec3 worldPosition)
{
    float shadowMapSize  = perApp.shadowVariables.z;

    if (!Is(perApp.shadowVariables.x) || shadowsBuffer.cascadesSize == 0)
        return 1.f;
        vec4 position = perFrame.projectionViewMatrix * vec4(worldPosition, 1.f);
        float z = position.z; // clipSpaceZ


    if (shadowsBuffer.cascadesSize >= 1 && z < shadowsBuffer.cascadesDistance.x)
    {
        vec4 positionInLightSpace = getPositionInLightSpace(0, worldPosition);
        return CalculateShadowFactorValue(shadowMap0, positionInLightSpace);
    }
    else if (shadowsBuffer.cascadesSize >= 2 && z < shadowsBuffer.cascadesDistance.y)
    {
        float blendRange = 2.0 / shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.x - blendRange,
                                  shadowsBuffer.cascadesDistance.x, z);

        vec4 positionInLightSpace0 = getPositionInLightSpace(0, worldPosition);
        vec4 positionInLightSpace1 = getPositionInLightSpace(1, worldPosition);

        float shadowPrev = CalculateShadowFactorValue(shadowMap0, positionInLightSpace0);
        float shadowCurr = CalculateShadowFactorValue(shadowMap1, positionInLightSpace1);
        return mix(shadowPrev, shadowCurr, factor);
    }
    else if (shadowsBuffer.cascadesSize >= 3 && z < shadowsBuffer.cascadesDistance.z)
    {
        float blendRange = 2.0 / shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.y - blendRange,
                                  shadowsBuffer.cascadesDistance.y, z);


        vec4 positionInLightSpace1 = getPositionInLightSpace(1, worldPosition);
        vec4 positionInLightSpace2 = getPositionInLightSpace(2, worldPosition);

        float shadowPrev = CalculateShadowFactorValue(shadowMap1, positionInLightSpace1);
        float shadowCurr = CalculateShadowFactorValue(shadowMap2, positionInLightSpace2);
        return mix(shadowPrev, shadowCurr, factor);
    }
    else if (shadowsBuffer.cascadesSize >= 4 && z < shadowsBuffer.cascadesDistance.w)
    {
        float blendRange = 2.0 / shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.z - blendRange,
                                  shadowsBuffer.cascadesDistance.z, z);

        vec4 positionInLightSpace2 = getPositionInLightSpace(2, worldPosition);
        vec4 positionInLightSpace3 = getPositionInLightSpace(3, worldPosition);

        float shadowPrev = CalculateShadowFactorValue(shadowMap2, positionInLightSpace2);
        float shadowCurr = CalculateShadowFactorValue(shadowMap3, positionInLightSpace3);
        return mix(shadowPrev, shadowCurr, factor);
    }

    return 1.f;
}

vec4 CalculateColor(SMaterial material, vec3 worldPosition, vec3 unitNormal)
{
    vec4 total_color = vec4(0.0,0.0,0.0,1.0);

    // Directional lights
    for (int i = 0; i < lightsPass.numberOfLights.x && i < MAX_DIR_LIGHTS; ++i)
    {
        vec3 dir =  normalize(-dirLights.direction[i].xyz);
        vec3 rgb = dirLights.color[i].rgb;
        float intensity = dirLights.color[i].a;
        vec3 light_color = rgb * intensity;
        total_color += CalculateBaseLight(material, dir, worldPosition, unitNormal, light_color);
    }

    // Point lights
    for (int i = 0; i < lightsPass.numberOfLights.y && i < MAX_POINT_LIGHTS; ++i)
    {
        total_color += CalculatePointLight(material, i, worldPosition, unitNormal);
    }

    // Spot lights
    for (int i = 0; i < lightsPass.numberOfLights.z && i < MAX_SPOT_LIGHTS; ++i)
    {
        total_color += CalcSpotLight(material, i, worldPosition, unitNormal);
    }

    return total_color;
}

float ToZBuffer(sampler2D tex, vec2 coord)
{
    const float near    = perFrame.projection.x;
    const float far     = perFrame.projection.y;
    const float depth   = texture(tex, coord).r;
    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

vec4 CreateFog(vec4 finalColor, float fogEnabled)
{
    if (fogEnabled > 0.5)
    {
        // distance-based fog/visibility:
        float viewDist = perApp.viewDistance.x; //lightsPass.viewDistance;
        float zView = ToZBuffer(DepthTexture, vs_in.textureCoords);
        float gradient = perApp.fogData.w;
        float visibility = exp(-pow((zView * (6 / max(1.0, viewDist))), gradient));
        visibility = clamp(visibility, 0.0, 1.0);
        vec3 sky = texture(SkyTexture, vs_in.textureCoords).rgb;
        return  mix(vec4(sky, 1.f), finalColor, visibility);
    }
    return finalColor;
}

void main()
{
    float depth = texture(DepthTexture, vs_in.textureCoords).r;
    if (depth >= 1.0)
    {
        vec3 sky = texture(SkyTexture, vs_in.textureCoords).rgb;
        FragColor = vec4(sky, 1.0);
        return;
    }

    vec4 normal4        = texture(NormalMap, vs_in.textureCoords);
    vec4 specular       = texture(SpecularMap, vs_in.textureCoords);
    vec3 worldPosition  = texture(PositionMap, vs_in.textureCoords).xyz;
    vec3 color          = texture(ColorMap, vs_in.textureCoords).xyz;
    vec3 normal         = normalize(normal4.xyz);

    SMaterial material;
    float maxValue = max3(color);
    float ambientFactor = 0.2;

    float shadowFactor      = CalculateShadowFactor(worldPosition);
    material.ambient_       = color * ambientFactor;
    material.diffuse_       = max(color - material.ambient_, vec3(0.0)) * shadowFactor;
    material.specular_      = specular.xyz;
    material.shineDamper_   = clamp(specular.a * 255.0, 1.0, 2048.0);

    vec4 finalColor = CalculateColor(material, worldPosition, normal);
    finalColor      = CreateFog(finalColor, normal4.a);
    FragColor       = vec4(finalColor.rgb, 1.0);
}
