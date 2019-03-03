//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D shadowMap : register(t0);
SamplerState shadowMapState : register(s0);

Texture2D blendMap : register(t1);
SamplerState blendMapState : register(s1);

Texture2D backgorundTexture : register(t2);
SamplerState backgorundTextureState : register(s2);

Texture2D backgorundTextureNormal : register(t3);
SamplerState backgorundTextureNormalState : register(s3);

Texture2D redTexture : register(t4);
SamplerState redTextureState : register(s4);

Texture2D redTextureNormal : register(t5);
SamplerState redTextureNormalState : register(s5);

Texture2D greenTexture : register(t6);
SamplerState greenTextureState : register(s6);

Texture2D greenTextureNormal : register(t7);
SamplerState greenTextureNormalState : register(s7);

Texture2D blueTexture : register(t8);
SamplerState blueTextureState : register(s8);

Texture2D blueTextureNormal : register(t9);
SamplerState blueTextureNormalState : register(s9);

Texture2D rockTexture : register(t10);
SamplerState rockTextureState : register(s10);

Texture2D rockTextureNormal : register(t11);
SamplerState rockTextureNormalState : register(s11);

Texture2D snowTexture : register(t12);
SamplerState snowTextureState : register(s12);

Texture2D snowTextureNormal : register(t13);
SamplerState snowTextureStateNormal : register(s13);

cbuffer PerApp : register(b0)
{
    float useTextures;
    float viewDistance;
    float3 shadowVariables;
    float4 clipPlane;
};

cbuffer PerFrame : register(b1)
{
    matrix projectionViewMatrix;
    matrix toShadowMapSpace;
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
    matrix bonesTransforms[25];
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

float GetScaledYNormal(float3 normal)
{
    float normalY = abs(normalize(normal).y);
    normalY = normalY * 2;
    if (normalY > 1)
        normalY = 1;
    return normalY;
}

float4 CalculateTerrainColor(float3 normal, float2 textCoord)
{
    float normalY = GetScaledYNormal(normal);
    float4 blendMapColour = blendMap.Sample(blendMapState, textCoord);
    float backTextureAmount = 1.f - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
    float2 tiledCoords = textCoord * 40.0f;
    float4 backgorundTextureColour = backgorundTexture.Sample(backgorundTextureState, tiledCoords) * backTextureAmount * normalY + (rockTexture.Sample(rockTextureState, tiledCoords) * backTextureAmount * (1.f - normalY));
    float4 redTextureColour = redTexture.Sample(redTextureState, tiledCoords) * blendMapColour.r;
    float4 greenTextureColour = greenTexture.Sample(greenTextureState, tiledCoords) * blendMapColour.g;
    float4 blueTextureColour = blueTexture.Sample(blueTextureState, tiledCoords) * blendMapColour.b;

    return backgorundTextureColour + redTextureColour + greenTextureColour + blueTextureColour;
}

float4 PS(PS_INPUT input)
    : SV_Target
{
    float4 samplerColor = float4(1, 1, 1, 1);
    samplerColor = CalculateTerrainColor(input.N, input.Tex);//backgorundTexture.Sample(backgorundTextureState, input.Tex);
    //samplerColor = backgorundTexture.Sample(backgorundTextureState, input.Tex);
    if (samplerColor.w < 0.5)
    {
        discard;
    }

    LightPass lightsPass;
    lightsPass.numberOfLights  = 1;
    lightsPass.type_[0]        = LIGHT_TYPE_DIRECTIONAL;
    lightsPass.position_[0]    = float3(100000, 150000, 100000);
    lightsPass.color_[0]       = float3(1, 1, 1);
    lightsPass.attenuation_[0] = float3(1, 0, 0);
    lightsPass.cutOff_[0]      = 0.f;

    Material material;
    material.ambient_     = float3(0.05, 0.05, 0.05) * samplerColor.xyz;
    material.diffuse_     = float3(0.8, 0.8, 0.8) * samplerColor.xyz;
    material.specular_    = float3(0.0, 0.0, 0.0);
    material.shineDamper_ = 20;

    return CalculateColor(lightsPass, material, input.WorldPos, input.N);
}
