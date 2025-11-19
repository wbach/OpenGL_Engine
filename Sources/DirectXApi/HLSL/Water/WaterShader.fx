//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
SamplerState linearSamplerState : register(s0);
Texture2D reflectionTexture : register(t0);
Texture2D refractionTexture : register(t1);
Texture2D normalMap : register(t2);
Texture2D dudvMap : register(t4);
Texture2D depthMap : register(t5);

cbuffer PerFrame : register(b1)
{
    matrix projectionViewMatrix;
    float3 cameraPosition;
    float4 clipPlane;
    float4 projection;
};

cbuffer PerObjectUpdate : register(b3)
{
    matrix transformMatrix;
};

cbuffer PerMeshObject : register(b6)
{
    float4 waterColor;
    float tiledValue;
    float isSimpleRender;
    float moveFactor;
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
    output.Pos      = mul(output.Pos, projectionViewMatrix);
    output.Tex      = input.Tex;
    output.Color    = waterColor;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input)
    : SV_Target
{
    return float4(input.Color.xyz, 0.5f);
}
