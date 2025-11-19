#version 440
#define MAX_LIGHTS              10
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#define LIGHT_TYPE_DIRECTIONAL  0
#define LIGHT_TYPE_POINT        1
#define LIGHT_TYPE_SPOT         2

const float gradient = 2.5;
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

layout (std140, align=16, binding=6) uniform LightPass
{
    vec4 skyColor;
    vec2 screenSize;
    float viewDistance;
    int numberOfLights;
    vec3 position_[MAX_LIGHTS];
    vec3 color_[MAX_LIGHTS];
    vec3 attenuation_[MAX_LIGHTS];
    vec3 direction_[MAX_LIGHTS];
    float cutOff_[MAX_LIGHTS];
    int type_[MAX_LIGHTS];
} lightsPass;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection; // x - near, y -far
} perFrame;

layout (std140,binding=7) uniform ShadowsBuffer
{
    mat4 directionalLightSpace[MAX_SHADOW_MAP_CASADES];
    vec4 cascadesDistance;
    float cascadesSize;
} shadowsBuffer;

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

vec4 CalculateBaseLight(SMaterial material, vec3 light_direction, vec3 world_pos, vec3 unit_normal, vec3 light_color)
{
    // light_direction is the vector from fragment TO light (L)
    float NdotL = max(dot(unit_normal, light_direction), 0.0);

    vec3 ambient_color   = material.ambient_;
    vec3 diffuse_color   = light_color * NdotL * material.diffuse_;
    vec3 specular_color  = vec3(0.0);

    if (length(material.specular_) > 0.001)
    {
        // view direction: from fragment TO camera
        vec3 viewDir = normalize(perFrame.cameraPosition - world_pos);
        // Blinn-Phong halfway vector
        vec3 H = normalize(viewDir + light_direction);
        float NdotH = max(dot(unit_normal, H), 0.0);

        float specFactor = pow(NdotH, max(material.shineDamper_, 1.0));
        specular_color = light_color * material.specular_ * specFactor;
    }

    return vec4(ambient_color + diffuse_color + specular_color, 1.0);
}

vec4 CalcDirectionalLight(SMaterial material, Light light, vec3 world_pos, vec3 unit_normal)
{
    // Use direction_ (unit) for directional lights. Convention: direction_ points *towards* light (i.e. direction of incoming light)
    vec3 lightDir = normalize(-light.direction_);
    return CalculateBaseLight(material, lightDir,  world_pos, unit_normal, light.color_);
}

vec4 CalculatePointLight(SMaterial material, Light light, vec3 world_pos, vec3 unit_normal)
{
    vec3 to_light_vector    = light.position_ - world_pos;
    float distance_to_light = length(to_light_vector);
    vec3 light_direction    = normalize(to_light_vector); // from fragment TO light (L)

    vec4 color = CalculateBaseLight(material, light_direction, world_pos, unit_normal, light.color_);

    float att_factor =  light.attenuation_.x +
                        light.attenuation_.y * distance_to_light +
                        light.attenuation_.z * distance_to_light * distance_to_light;

    att_factor = max(att_factor, 0.0001); // avoid div by zero
    return color / att_factor;
}

vec4 CalcSpotLight(SMaterial material, Light light, vec3 world_pos, vec3 unit_normal)
{
    vec3 to_light_vector = light.position_ - world_pos;
    float distance_to_light = length(to_light_vector);
    vec3 L = normalize(to_light_vector); 
    vec3 spotDir = normalize(light.direction_);
    float cosTheta = dot(-L, spotDir);

    if (cosTheta > light.cutOff_)
    {
        float epsilon = 0.01;
        float intensity = smoothstep(light.cutOff_, light.cutOff_ + epsilon, cosTheta);

        vec4 color = CalculatePointLight(material, light, world_pos, unit_normal);
        return color * intensity;
    }
    else
    {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }
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

vec4 getPositionInLightSpace(int index, vec3 worldPos)
{
    return shadowsBuffer.directionalLightSpace[index] * vec4(worldPos, 1.0);
}

float CalculateShadowFactor(vec3 worldPos)
{
    float shadowMapSize  = perApp.shadowVariables.z;

    if (!Is(perApp.shadowVariables.x) || shadowsBuffer.cascadesSize == 0)
        return 1.f;
        vec4 position = perFrame.projectionViewMatrix * vec4(worldPos, 1.f);
        float z = position.z; // clipSpaceZ


    if (shadowsBuffer.cascadesSize >= 1 && z < shadowsBuffer.cascadesDistance.x)
    {
        vec4 positionInLightSpace = getPositionInLightSpace(0, worldPos);
        return CalculateShadowFactorValue(shadowMap0, positionInLightSpace);
    }
    else if (shadowsBuffer.cascadesSize >= 2 && z < shadowsBuffer.cascadesDistance.y)
    {
        float blendRange = 2.0 / shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.x - blendRange,
                                  shadowsBuffer.cascadesDistance.x, z);

        vec4 positionInLightSpace0 = getPositionInLightSpace(0, worldPos);
        vec4 positionInLightSpace1 = getPositionInLightSpace(1, worldPos);

        float shadowPrev = CalculateShadowFactorValue(shadowMap0, positionInLightSpace0);
        float shadowCurr = CalculateShadowFactorValue(shadowMap1, positionInLightSpace1);
        return mix(shadowPrev, shadowCurr, factor);
    }
    else if (shadowsBuffer.cascadesSize >= 3 && z < shadowsBuffer.cascadesDistance.z)
    {
        float blendRange = 2.0 / shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.y - blendRange,
                                  shadowsBuffer.cascadesDistance.y, z);


        vec4 positionInLightSpace1 = getPositionInLightSpace(1, worldPos);
        vec4 positionInLightSpace2 = getPositionInLightSpace(2, worldPos);

        float shadowPrev = CalculateShadowFactorValue(shadowMap1, positionInLightSpace1);
        float shadowCurr = CalculateShadowFactorValue(shadowMap2, positionInLightSpace2);
        return mix(shadowPrev, shadowCurr, factor);
    }
    else if (shadowsBuffer.cascadesSize >= 4 && z < shadowsBuffer.cascadesDistance.w)
    {
        float blendRange = 2.0 / shadowMapSize;
        float factor = smoothstep(shadowsBuffer.cascadesDistance.z - blendRange,
                                  shadowsBuffer.cascadesDistance.z, z);

        vec4 positionInLightSpace2 = getPositionInLightSpace(2, worldPos);
        vec4 positionInLightSpace3 = getPositionInLightSpace(3, worldPos);

        float shadowPrev = CalculateShadowFactorValue(shadowMap2, positionInLightSpace2);
        float shadowCurr = CalculateShadowFactorValue(shadowMap3, positionInLightSpace3);
        return mix(shadowPrev, shadowCurr, factor);
    }

    return 1.f;
}

vec4 CalculateColor(SMaterial material, vec3 world_pos, vec3 unit_normal)
{
    vec4 total_color = vec4(0.0,0.0,0.0,1.0);

    for (int i = 0; i < lightsPass.numberOfLights; i++)
    {
        Light light;
        light.attenuation_ = lightsPass.attenuation_[i];
        light.position_ = lightsPass.position_[i];
        light.color_ = lightsPass.color_[i];
        light.type_ = lightsPass.type_[i];
        light.cutOff_ = lightsPass.cutOff_[i];
        light.direction_ = lightsPass.direction_[i];

        if (light.type_ == LIGHT_TYPE_DIRECTIONAL)
            total_color += CalcDirectionalLight(material, light, world_pos, unit_normal);
        else if (light.type_ == LIGHT_TYPE_POINT)
            total_color += CalculatePointLight(material, light, world_pos, unit_normal);
        else if (light.type_ == LIGHT_TYPE_SPOT)
            total_color += CalcSpotLight(material, light, world_pos, unit_normal);
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

    vec4 normal4    = texture(NormalMap, vs_in.textureCoords);
    vec4 specular   = texture(SpecularMap, vs_in.textureCoords);
    vec3 world_pos  = texture(PositionMap, vs_in.textureCoords).xyz;
    vec3 color      = texture(ColorMap, vs_in.textureCoords).xyz;
    vec3 normal     = normalize(normal4.xyz);

    SMaterial material;
    float maxValue = max3(color);
    float ambientFactor = 0.1;

    float shadowFactor = CalculateShadowFactor(world_pos);

    material.ambient_ = color * ambientFactor;
    material.diffuse_ = max(color - material.ambient_, vec3(0.0)) * shadowFactor;
    material.specular_ = specular.xyz;
    material.shineDamper_ = clamp(specular.a * 255.0, 1.0, 2048.0);

    vec4 finalColor = CalculateColor(material, world_pos, normal);
    finalColor = CreateFog(finalColor, normal4.a);
    FragColor = vec4(finalColor.rgb, 1.0);
}
