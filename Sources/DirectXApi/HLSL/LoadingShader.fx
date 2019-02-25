//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer PerObjectUpdate : register(b3)
{
    matrix transformMatrix;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos      = float4(input.Pos, 1);
    output.Pos      = mul(output.Pos, transformMatrix);
    output.Tex      = input.Tex;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input)
    : SV_Target
{
    float4 color = txDiffuse.Sample(samLinear, input.Tex);
    if (color.w < 0.5)
    {
        discard;
    }
    return color;
}
