//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer PerApp : register( b0 )
{
    float useTextures;
    float viewDistance;
    float3 shadowVariables;
    float4 clipPlane;
};

cbuffer PerResize : register( b1 )
{
    matrix projectionMatrix;
};

cbuffer PerFrame : register( b2 )
{
    matrix viewMatrix;
    matrix toShadowMapSpace;
};

cbuffer PerObjectConstants : register( b3 )
{
    float useBoneTransform;
    float2 textureOffset;
};

cbuffer PerObjectUpdate : register( b4 )
{
    matrix transformationMatrix;
};

cbuffer PerPoseUpdate : register( b5 )
{
    matrix bonesTransforms[25];
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
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
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, transformationMatrix );
    output.Pos = mul( output.Pos, viewMatrix );
    output.Pos = mul( output.Pos, projectionMatrix );
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return float4( 0.8, 0.8, 0.8, 1.0);
}
