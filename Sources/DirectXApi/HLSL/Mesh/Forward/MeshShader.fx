//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

static const int MAX_BONES   = 512;
static const int MAX_WEIGHTS = 4;

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
    matrix bonesTransforms[MAX_BONES];
};

cbuffer PerMeshObject : register(b6)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    uint numberOfRows;
    float haveDiffTexture;
    float haveNormalMap;
    float haveSpecularMap;
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
    float4 boneWeights : BONEWEIGHTS;
    int4 boneIds : BONEIDS;
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

struct VertexWorldData
{
    float4 worldPosition;
    float4 worldNormal;
};

bool Is(float v)
{
    return v > 0.5f;
}

VertexWorldData caluclateWorldData(VS_INPUT input)
{
    VertexWorldData result;
    result.worldPosition = float4(0.0, 0.0, 0.0, 0.0);
    result.worldNormal   = float4(0.0, 0.0, 0.0, 0.0);

    if (!Is(useBoneTransform))
    {
        result.worldPosition = mul(float4(input.Pos, 1.0), transformationMatrix);
        result.worldNormal   = mul(float4(input.normal, 0.0), transformationMatrix);
        return result;
    }

    for (int i = 0; i < MAX_WEIGHTS; i++)
    {
        matrix boneTransform = bonesTransforms[input.boneIds[i]];
        float4 posePosition  = mul(float4(input.Pos, 1.0), boneTransform);
        result.worldPosition += posePosition * input.boneWeights[i];

        float4 worldNormal = mul(float4(input.normal, 0.0), boneTransform);
        result.worldNormal += worldNormal * input.boneWeights[i];
    }

    result.worldPosition = mul(result.worldPosition, transformationMatrix);
    result.worldNormal   = mul(result.worldNormal, transformationMatrix);
    return result;
}

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output                 = (PS_INPUT)0;
    VertexWorldData vertexWorldData = caluclateWorldData(input);

    output.WorldPos = output.Pos.xyz;
    output.Pos      = mul(vertexWorldData.worldPosition, projectionViewMatrix);
    output.Tex      = input.Tex;
    output.N        = normalize(vertexWorldData.worldNormal.xyz);
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

float4 PS(PS_INPUT input)
    : SV_Target
{
    float4 samplerColor = txDiffuse.Sample(samLinear, input.Tex);

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
    material.ambient_     = float3(0.2, 0.2, 0.2) * samplerColor.xyz;
    material.diffuse_     = float3(0.8, 0.8, 0.8) * samplerColor.xyz;
    material.specular_    = float3(0.0, 0.0, 0.0);
    material.shineDamper_ = 20;

    return CalculateColor(lightsPass, material, input.WorldPos, input.N);
}
