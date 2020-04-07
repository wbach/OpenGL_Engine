//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer PerObjectUpdate : register(b3)
{
    matrix transformMatrix;
};

cbuffer PerMeshObject : register(b6)
{
    float4 inputTextureColor;
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
    float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos      = float4(input.Pos, 1);
    output.Pos      = mul(output.Pos, transformMatrix);
    output.Tex      = float2((input.Pos.x + 1.0) / 2.0, 1 - (input.Pos.y + 1.0) / 2.0);
    output.Color    = inputTextureColor;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input)
    : SV_Target
{
    float4 color = txDiffuse.Sample(samLinear, input.Tex) * input.Color;
    return color;
}
