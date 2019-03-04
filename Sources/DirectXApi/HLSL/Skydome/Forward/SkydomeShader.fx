//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

cbuffer PerFrame : register(b1)
{
    matrix projectionViewMatrix;
    matrix toShadowMapSpace;
    float3 cameraPosition;
};

cbuffer PerObjectUpdate : register(b3)
{
    matrix transformationMatrix;
};

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
    float4 WorldPos : POSITION1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = float4(input.Pos, 1);
    output.WorldPos = mul(output.Pos, transformationMatrix);
    output.Pos = mul(output.WorldPos, projectionViewMatrix);
    return output;
}

static const float3 baseColor = float3(0.18, 0.27, 0.47);

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input)
    : SV_Target
{
    float red = -0.00022f * (abs(input.WorldPos.y) - 2800) + baseColor.x;
    float green = -0.00025f * (abs(input.WorldPos.y) - 2800) + baseColor.y;
    float blue = -0.00019f * (abs(input.WorldPos.y) - 2800) + baseColor.z;

    return float4(red, green, blue, 1.f);
}
