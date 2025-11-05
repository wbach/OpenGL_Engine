#version 440
#define MAX_LIGHTS              10
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#define LIGHT_TYPE_DIRECTIONAL  0
#define LIGHT_TYPE_POINT        1
#define LIGHT_TYPE_SPOT         2

const float density = 0.0025;
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
};

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
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
    float cutOff_[MAX_LIGHTS];
    int type_[MAX_LIGHTS];
} lightsPass;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
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
layout(binding = 5) uniform sampler2DShadow shadowMap0;
layout(binding = 6) uniform sampler2DShadow shadowMap1;
layout(binding = 7) uniform sampler2DShadow shadowMap2;
layout(binding = 8) uniform sampler2DShadow shadowMap3;

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
vec4 CalculateBaseLight(SMaterial material, vec3 light_direction, vec3 world_pos, vec3 unit_normal, vec3 light_color)
{
    float diffuse_factor = dot(light_direction, unit_normal);

    vec3 ambient_color   = vec3(0.f);
    vec3 diffuse_color   = vec3(0.f);
    vec3 specular_color  = vec3(0.f);

    ambient_color =  material.ambient_;

    if (diffuse_factor > 0.f)
    {
        diffuse_color = light_color * diffuse_factor;
        //ambient_color = diffuse_color * 0.2f;
    }
    else
    {
        // for sobel filter?
       // diffuse_color = light_color * -1.f * diffuse_factor;
    }

    diffuse_color = diffuse_color * material.diffuse_;

    if (length(material.specular_) > .01f)
    {
        vec3    vertex_to_camera    = normalize(world_pos - vs_in.cameraPosition);
        vec3    light_reflect       = normalize(reflect(light_direction, unit_normal));
        float   specular_factor     = dot(vertex_to_camera, light_reflect);
                specular_factor     = pow(specular_factor, material.shineDamper_);

        if (specular_factor > .0f && specular_factor < 90.f* M_PI/180.f)
        {
            specular_color = light_color * material.specular_ * specular_factor;
        }
    }

    return vec4(ambient_color, 1.f) + vec4(diffuse_color, 1.f) + vec4(specular_color, 1.f);

}

vec4 CalcDirectionalLight(SMaterial material, Light light, vec3 world_pos, vec3 unit_normal)
{
    vec3 to_light_vector = light.position_ - world_pos;
    vec3 light_direction = normalize(to_light_vector);

    return CalculateBaseLight(material, light_direction, world_pos, unit_normal, light.color_);
}

vec4 CalculatePointLight(SMaterial material, Light light, vec3 world_pos, vec3 unit_normal)
{

    vec3 to_light_vector    = light.position_ - world_pos;
    float distance_to_light = length(to_light_vector);
    vec3 light_direction    = normalize(to_light_vector);

    vec4 color = CalculateBaseLight(material, light_direction, world_pos, unit_normal, light.color_);

    float att_factor =  light.attenuation_.x +
                        light.attenuation_.y * distance_to_light +
                        light.attenuation_.z * distance_to_light * distance_to_light;

    return color / att_factor;
}

vec4 CalcSpotLight(SMaterial material, Light light, vec3 world_pos, vec3 unit_normal)
{
    vec3 to_light_vector = light.position_ - world_pos;
    vec3 light_direction = normalize(to_light_vector);

  //  vec3 light_to_pixel = normalize(WorldPos0 - l.Base.Position);
    float spot_factor = dot(light_direction, light_direction);

    if (spot_factor > light.cutOff_)
    {
        vec4 color = CalculatePointLight(material, light, world_pos, unit_normal);
        return color * (1.f - (1.f - spot_factor) * 1.f/(1.f - light.cutOff_));
    }
    else
    {
        return vec4(0.f, 0.f, 0.f, 1.f);
    }
}

vec4 CalculateColor(SMaterial material, vec3 world_pos, vec3 unit_normal)
{
    vec4 total_color = vec4(0.f, 0.f, 0.f, 1.f);

    for (int i = 0; i < lightsPass.numberOfLights; i++)
    {
        Light light;
        light.attenuation_ = lightsPass.attenuation_[i];
        light.position_ = lightsPass.position_[i];
        light.color_ = lightsPass.color_[i];
        light.type_ = lightsPass.type_[i];
        light.cutOff_ = lightsPass.cutOff_[i];

        switch(light.type_)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                total_color += CalcDirectionalLight(material, light, world_pos, unit_normal);
                break;
            case LIGHT_TYPE_POINT:
                total_color += CalculatePointLight(material, light, world_pos, unit_normal);
                break;
            case LIGHT_TYPE_SPOT:
                total_color += CalcSpotLight(material, light, world_pos, unit_normal);
                break;
        }
    }

    return total_color;
}

float ToZBuffer(sampler2D tex, vec2 coord)
{
    float zNear = 0.1f;
    float zFar = 1000.f;
    float z_b = texture(tex, coord).x;
    float z_n = 2.0 * z_b - 1.0;
    float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
    return z_e;
}

float max3(vec3 v)
{
  return max(max (v.x, v.y), v.z);
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
    return shadowsBuffer.directionalLightSpace[index] * vec4(worldPos, 1.f);
}

float CalculateShadowFactor2(vec3 worldPos)
{
    bool useShadows   = perApp.shadowVariables.x > 0.5f;
    if (useShadows && shadowsBuffer.cascadesSize > 0)
    {
        vec4 position = perFrame.projectionViewMatrix * vec4(worldPos, 1.f);
        float distanceToCamera = position.z; // clipSpaceZ

        if (distanceToCamera < shadowsBuffer.cascadesDistance.x)
        {
            vec4 positionInLightSpace = getPositionInLightSpace(0, worldPos);
            return CalculateShadowFactorValue(shadowMap0, positionInLightSpace);
        }
        else if (distanceToCamera < shadowsBuffer.cascadesDistance.y)
        {
            vec4 positionInLightSpace = getPositionInLightSpace(1, worldPos);
            return CalculateShadowFactorValue(shadowMap1, positionInLightSpace);
        }
        else if (distanceToCamera < shadowsBuffer.cascadesDistance.z)
        {
            vec4 positionInLightSpace = getPositionInLightSpace(2, worldPos);
            return CalculateShadowFactorValue(shadowMap2, positionInLightSpace);
        }
        else if (distanceToCamera < shadowsBuffer.cascadesDistance.w)
        {
            vec4 positionInLightSpace = getPositionInLightSpace(3, worldPos);
            return CalculateShadowFactorValue(shadowMap3, positionInLightSpace);
        }
        else
        {
            return 1.f;
        }
    }
    else
    {
        return 1.f;
    }
}
bool Is(float v)
{
    return v > 0.5f;
}
float CalculateShadowFactor(vec3 worldPos)
{
    float shadowMapSize  = perApp.shadowVariables.z;

    if (!Is(perApp.shadowVariables.x) || shadowsBuffer.cascadesSize == 0)
        return 1.f;

        vec4 position = perFrame.projectionViewMatrix * vec4(worldPos, 1.f);
        float z = position.z; // clipSpaceZ


    // Kaskada 1
    if (shadowsBuffer.cascadesSize >= 1 && z < shadowsBuffer.cascadesDistance.x)
    {
        vec4 positionInLightSpace = getPositionInLightSpace(0, worldPos);
        return CalculateShadowFactorValue(shadowMap0, positionInLightSpace);
    }
    // Kaskada 2 z płynnym przejściem
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
    // Kaskada 3
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
    // Kaskada 4
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

void main()
{
  //  FragColor = texture(ColorMap, tex_coord); return;
    float distance  = ToZBuffer(DepthTexture, vs_in.textureCoords);

    vec4 normal4    = texture(NormalMap, vs_in.textureCoords);
    vec4 specular   = texture(SpecularMap, vs_in.textureCoords);
    vec3 world_pos  = texture(PositionMap, vs_in.textureCoords).xyz;
    vec3 color      = texture(ColorMap, vs_in.textureCoords).xyz;
    vec3 normal     = normalize(normal4.xyz);

    SMaterial material;
    float maxValue = max3(color);
    float norm = 1.f - maxValue;
    float ambientFactor = 0.1f;

    float shadowFactor = CalculateShadowFactor(world_pos);
    material.ambient_ = (color + vec3(maxValue, maxValue, maxValue)) * ambientFactor;
    material.diffuse_ = (color - material.ambient_) * shadowFactor;
    material.specular_ = specular.xyz;
    material.shineDamper_ = specular.a * 255.f;

    vec4 finalColor;
    float visibility = 1.f;

    if (normal4.a > .5f)
    {
        visibility = exp(-pow((distance*( ( 45.0f / lightsPass.viewDistance))), gradient));
        visibility = clamp(visibility, 0.0f, 1.0f) ;
        finalColor = CalculateColor(material, world_pos, normal);
    }
    else
    {
        visibility = 1.f;
        finalColor = vec4(color, 1.f);
    }

    finalColor = mix(lightsPass.skyColor, finalColor, visibility);

    const float gamma = 1.8f;
    finalColor = pow(finalColor, vec4(1.f / gamma));

     FragColor = vec4(finalColor.rgb, 1.f);

   const float contrast = 0.15f;
   FragColor.rgb = (FragColor.rgb - .5f) * (1.f + contrast) + .5f;
   FragColor     = mix(vec4(0.8), FragColor, visibility);


//return;

    //FragColor = vec4(0, lightsPass.lights[0].type_ == 0, 0, 1.f);
    //FragColor = vec4(lightsPass.lights[0].color_, 1.f);
}
