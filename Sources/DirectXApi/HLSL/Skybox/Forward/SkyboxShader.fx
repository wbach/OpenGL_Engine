//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
SamplerState linearSamplerState : register(s0);
TextureCube DayCubeMap : register(t0);
TextureCube NightCubeMap : register(t1);

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

cbuffer PerObjectUpdate : register(b3)
{
    matrix transformationMatrix;
};

cbuffer PerMeshObject : register(b6)
{
    float blendFactor;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 boneWeights : BONEWEIGHTS;
    int3 boneIds : BONEIDS;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION1;
    float3 textureCoords : POSITION2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.position = float4(input.Pos, 1);
    output.worldPosition = mul(output.position, transformationMatrix);
    output.position = mul(output.worldPosition, projectionViewMatrix);
    output.textureCoords = input.Pos;
    return output;
}

const float LowerLimit = 0.0;
const float UpperLimit = 0.1;

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 textureColor(TextureCube inputTexture, float3 textCoord)
{
    return inputTexture.Sample(linearSamplerState, textCoord);
}

float4 PS(PS_INPUT input)
    : SV_Target
{
    float4 texture1   = textureColor(DayCubeMap, input.textureCoords);
    float4 texture2   = textureColor(NightCubeMap, input.textureCoords);
    float4 finalColor = lerp(texture2, texture1, blendFactor);
    return finalColor;

    float factor  = (input.textureCoords.y - LowerLimit)/(UpperLimit - LowerLimit );
    factor        = saturate(factor);  // clamp(factor, 0.f, 1.f)
    float4 output = lerp(float4(fogData.xyz, 1.f), finalColor, factor); // mix
    output.w = 1.f;
    return output;
}
