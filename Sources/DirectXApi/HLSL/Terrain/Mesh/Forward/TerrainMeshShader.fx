//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
SamplerState linearSamplerState : register(s0);
Texture2D shadowMap : register(t0);
Texture2D heightMap : register(t1);
Texture2D blendMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D backgroundTexture : register(t4);
Texture2D backgroundTextureNormal : register(t5);
Texture2D backgroundTextureDisplacment : register(t6);
Texture2D redTexture : register(t7);
Texture2D redTextureNormal : register(t8);
Texture2D redTextureDisplacement : register(t9);
Texture2D greenTexture : register(t10);
Texture2D greenTextureNormal : register(t11);
Texture2D greenTextureDisplacement : register(t12);
Texture2D blueTexture : register(t13);
Texture2D blueTextureNormal : register(t14);
Texture2D blueTextureDisplacement : register(t15);
Texture2D alphaTexture : register(t16);
Texture2D alphaTextureNormal : register(t17);
Texture2D alphaTextureDisplacement : register(t18);
Texture2D rockTexture : register(t19);
Texture2D rockTextureNormal : register(t20);
Texture2D rockTextureDisplacement : register(t21);

cbuffer PerApp : register(b0)
{
    float4 useTextures;
    float4 viewDistance;
    float4 shadowVariables;
    float4 fogData;
};

cbuffer PerFrame : register(b1)
{
    matrix projectionViewMatrix;
    float3 cameraPosition;
};

cbuffer PerObjectConstants : register(b2)
{
    float useBoneTransform;
    float2 textureOffset;
};

cbuffer PerObjectUpdate : register(b3)
{
    matrix transformationMatrix;
};

cbuffer PerPoseUpdate : register(b4)
{
    matrix bonesTransforms[100];
};

cbuffer PerMeshObject : register(b6)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    uint numberOfRows;
    float useTexture;
    float useNormalMap;
    float useSpecularMap;
    float shineDamper;
    float useFakeLighting;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 boneWeights : BONEWEIGHTS;
    int3 boneIds : BONEIDS;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 N : NORMAL;
    float3 WorldPos : POSITION1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos      = float4(input.Pos, 1);
    output.Pos      = mul(output.Pos, transformationMatrix);
    output.WorldPos = output.Pos.xyz;
    output.Pos      = mul(output.Pos, projectionViewMatrix);
    output.Tex      = input.Tex;
    output.N        = normalize(mul(float4(input.normal, 0.f), transformationMatrix).xyz);
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
#define MAX_LIGHTS 10
#define M_PI 3.14159265358979323846264338327950288 /* pi */
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Material
{
    float3 ambient_;
    float3 diffuse_;
    float3 specular_;
    float shineDamper_;
};

struct Light
{
    float3 position_;
    float3 color_;
    float3 attenuation_;
    float cutOff_;
    int type_;
};

struct LightPass
{
    float4 skyColor;
    float2 screenSize;
    float viewDistance;
    int numberOfLights;
    float3 position_[MAX_LIGHTS];
    float3 color_[MAX_LIGHTS];
    float3 attenuation_[MAX_LIGHTS];
    float cutOff_[MAX_LIGHTS];
    int type_[MAX_LIGHTS];
};

float4 CalculateBaseLight(Material material, float3 light_direction, float3 world_pos, float3 unit_normal,
                          float3 light_color)
{
    float diffuse_factor = dot(unit_normal, light_direction);

    float3 ambient_color  = float3(0.0, 0.0, 0.0);
    float3 diffuse_color  = float3(0.0, 0.0, 0.0);
    float3 specular_color = float3(0.0, 0.0, 0.0);

    if (diffuse_factor > 0.0)
    {
        diffuse_color = light_color * diffuse_factor;
    }
    // ambient color
    diffuse_color = diffuse_color * material.diffuse_;
    ambient_color = material.ambient_;

    // if (material.shineDamper_ > 0.0)
    //{
    //    float3 vertex_to_camera = normalize(world_pos - vs_in.cameraPosition);
    //    float3 light_reflect    = normalize(reflect(light_direction, unit_normal));
    //    float specular_factor = dot(vertex_to_camera, light_reflect);
    //    specular_factor       = pow(specular_factor, material.shineDamper_);
    //    if (specular_factor > 0.0 && specular_factor < 90.0 * M_PI / 180.0)
    //    {
    //        specular_color = light_color * material.specular_ * specular_factor;
    //    }
    //}

    return float4(ambient_color, 0.0) + float4(diffuse_color, 0.0) + float4(specular_color, 0.0);
}

float4 CalcDirectionalLight(Material material, Light light, float3 world_pos, float3 unit_normal)
{
    float3 to_light_vector = light.position_ - world_pos;
    float3 light_direction = normalize(to_light_vector);

    return CalculateBaseLight(material, light_direction, world_pos, unit_normal, light.color_);
}

float4 CalculateColor(LightPass lightsPass, Material material, float3 world_pos, float3 unit_normal)
{
    float4 total_color = float4(0.0, 0.0, 0.0, 1.0);

    for (int i = 0; i < lightsPass.numberOfLights; i++)
    {
        Light light;
        light.attenuation_ = lightsPass.attenuation_[i];
        light.position_    = lightsPass.position_[i];
        light.color_       = lightsPass.color_[i];
        light.type_        = lightsPass.type_[i];
        light.cutOff_      = lightsPass.cutOff_[i];

        switch (light.type_)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                total_color += CalcDirectionalLight(material, light, world_pos, unit_normal);
                break;
            case LIGHT_TYPE_POINT:
                // total_color += CalculatePointLight(material, light, world_pos, unit_normal);
                break;
            case LIGHT_TYPE_SPOT:
                // total_color += CalcSpotLight(material, light, world_pos, unit_normal);
                break;
        }
    }

    return total_color;
}

struct TerrainData
{
    float4 color;
    float4 normal;
};

float4 textureColor(Texture2D texture2d, float2 textCoord)
{
    return texture2d.Sample(linearSamplerState, textCoord);
}

bool Is(float f)
{
    return f > .5f;
}

static const float2 thresholds = float2(.5f, .1f);

float GetRockBlendFactor(float normalY)
{
    float value = 1.f - normalY;

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

float4 CalculateBackgroundColor(float2 tiledCoords, float backTextureAmount, float normalY)
{
    float blendFactor = GetRockBlendFactor(normalY);
    float4 backgroundTextureColor = textureColor(backgroundTexture, tiledCoords) * backTextureAmount * (1.f - blendFactor);
    float4 rockTextureColor       = textureColor(rockTexture, tiledCoords) * backTextureAmount * blendFactor;
    return backgroundTextureColor + rockTextureColor;
}

float4 CalculateTerrainColor(float2 tiledCoords, float4 blendMapColor, float backTextureAmount, float normalY)
{
    // if (!Is(perApp.useTextures.x))
    // {
    //     return vec4(.8f, .8f, .8f, 1.f);
    // }

    float4 backgroundTextureColour = CalculateBackgroundColor(tiledCoords, backTextureAmount, normalY);
    float4 redTextureColor         = textureColor(redTexture, tiledCoords) * blendMapColor.r;
    float4 greenTextureColor       = textureColor(greenTexture, tiledCoords) * blendMapColor.g;
    float4 blueTextureColor        = textureColor(blueTexture, tiledCoords) * blendMapColor.b;
    float4 alphaTextureColor       = textureColor(alphaTexture, tiledCoords) * blendMapColor.a;
    return backgroundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}


// float4 CalculateTerrainColor(float3 normal, float2 textCoord)
// {
//     float4 blendMapColour = blendMap.Sample(linearSamplerState, textCoord);
//     float backTextureAmount = 1.f - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
//     float2 tiledCoords = textCoord * 40.0f;
//     float4 backgroundTextureColour = backgroundTexture.Sample(linearSamplerState, tiledCoords) * backTextureAmount;
//     float4 redTextureColour = redTexture.Sample(linearSamplerState, tiledCoords) * blendMapColour.r;
//     float4 greenTextureColour = greenTexture.Sample(linearSamplerState, tiledCoords) * blendMapColour.g;
//     float4 blueTextureColour = blueTexture.Sample(linearSamplerState, tiledCoords) * blendMapColour.b;

//     return backgroundTextureColour + redTextureColour + greenTextureColour + blueTextureColour;
// }

TerrainData GetTerrainData(float3 normal, float2 texCoord)
{
    float2 tiledCoords   = texCoord * 480.0f ;
    float4 blendMapColor = textureColor(blendMap, texCoord);
    float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);

    TerrainData result;
    result.color  = CalculateTerrainColor(tiledCoords, blendMapColor, backTextureAmount, normal.y);
    result.normal = float4(normal, 0.f);//CalculateTerrainNormal(tiledCoords, blendMapColor, backTextureAmount);
    return result;
}

float4 PS(PS_INPUT input)
    : SV_Target
{
    //return float4(1, 1, 1, 1);
    TerrainData terrainData = GetTerrainData(input.N, input.Tex);

    LightPass lightsPass;
    lightsPass.numberOfLights  = 1;
    lightsPass.type_[0]        = LIGHT_TYPE_DIRECTIONAL;
    lightsPass.position_[0]    = float3(100000, 150000, 100000);
    lightsPass.color_[0]       = float3(1, 1, 1);
    lightsPass.attenuation_[0] = float3(1, 0, 0);
    lightsPass.cutOff_[0]      = 0.f;

    Material material;
    material.ambient_     = float3(0.05, 0.05, 0.05) * terrainData.color.xyz;
    material.diffuse_     = float3(0.8, 0.8, 0.8) * terrainData.color.xyz;
    material.specular_    = float3(0.0, 0.0, 0.0);
    material.shineDamper_ = 20;

    return CalculateColor(lightsPass, material, input.WorldPos, input.N);
}
