//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

cbuffer SkyBuffer : register(b6)
{
    float4x4 invProj;
    float4x4 invViewRot;
    float4 screenSize;    // xy = resolution, zw = unused

    float4 sunDirection;  // xyz = direction, w = exists flag
    float4 sunColor;      // rgb = sun/moon color, a = isDayFactor (0 night, 1 day)
};

//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// --- CONFIG ---
static const float SUN_SIZE       = 0.009;
static const float SUN_INTENSITY  = 1.0;
static const float HALO_INTENSITY = 0.7;

//--------------------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------------------
float smoothSunDisk(float cosAngle, float angularRadius)
{
    float inner = cos(angularRadius);
    float outer = cos(angularRadius * 1.1);
    return smoothstep(outer, inner, cosAngle);
}

float hash12(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.x + p3.y) * p3.z);
}

float starField(float3 worldRay)
{
    float theta = acos(clamp(worldRay.y, -1.0, 1.0));
    float phi = atan2(worldRay.z, worldRay.x);

    float scale = 400.0;
    float2 cell = floor(float2(theta, phi) * scale);

    float h = hash12(cell);
    return step(0.9991, h);
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(uint vID : SV_VertexID)
{
    PS_INPUT output = (PS_INPUT)0;

    // Generowanie pełnoekranowego trójkąta (Triangle Hack)
    float x = -1.0 + (float)((vID & 1) << 2);
    float y = -1.0 + (float)((vID & 2) << 1);

    output.position = float4(x, y, 0.0, 1.0);
    output.uv = float2((x + 1.0) * 0.5, (y + 1.0) * 0.5);
    
    // W DX UV(0,0) to lewy górny róg. Odwracamy Y, aby pasowało do rekonstrukcji promienia.
    output.uv.y = 1.0 - output.uv.y;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
   // return float4(input.uv.x, input.uv.y, 0, 1);
    
    float2 uv = input.uv;

    // --- RECONSTRUCT WORLD RAY ---
    // NDC w DX: x[-1,1], y[-1,1]
    float2 ndcXY = input.uv * 2.0 - 1.0;
    float4 ndc = float4(ndcXY.x, ndcXY.y, 1.0, 1.0);
    float4 viewRay = mul(invProj, ndc);
    viewRay /= viewRay.w;
    
    float3 worldRay = normalize(mul(invViewRot, float4(viewRay.xyz, 0.0)).xyz);

    // --- CASE: NO DIRECTIONAL LIGHT ---
    if (sunDirection.w < 0.5)
    {
        float t = max(worldRay.y * 0.5 + 0.5, 0.0);
        float3 topColor = float3(0.1, 0.3, 0.8);
        float3 bottomColor = float3(0.6, 0.8, 1.0);
        float3 skyGradient = lerp(bottomColor, topColor, t);

        float horizonFactor = pow(saturate(worldRay.y), 1.1);
        float3 stars = float3(starField(worldRay), starField(worldRay), starField(worldRay)) * horizonFactor;

        return float4(skyGradient + stars, 1.0);
    }

    // --- SUN / MOON SETTINGS ---
    float3 sunDir = normalize(-sunDirection.xyz);
    float3 sunCol = sunColor.rgb;
    float dayFactor = sunColor.a;

    float elevation = saturate(dot(sunDir, float3(0, 1, 0)));
    float t = saturate(worldRay.y * 0.5 + 0.5);
    float phase = elevation * dayFactor;

    // --- SKY GRADIENT ---
    float3 zenith = lerp(float3(0.02, 0.05, 0.10), float3(0.10, 0.35, 0.95), phase);
    float3 horizon = lerp(float3(0.03, 0.04, 0.08), float3(0.6, 0.8, 1.0), phase);

    float sunsetFactor = (1.0 - elevation) * dayFactor;
    horizon = lerp(horizon, sunCol * float3(1.4, 1.0, 0.7), sunsetFactor);

    float3 sky = lerp(horizon, zenith, t);

    // --- SUN / MOON RENDER ---
    float cosAng = dot(worldRay, sunDir);
    float sunDisk = smoothSunDisk(cosAng, SUN_SIZE);

    float haloPow = lerp(32.0, 128.0, dayFactor);
    float haloStrength = lerp(0.2, 0.7, dayFactor);
    float sunHalo = pow(max(cosAng, 0.0), haloPow);

    float3 sunDayColor = lerp(float3(1.0, 0.6, 0.25), sunCol, elevation);
    float3 moonColor = float3(0.7, 0.75, 1.0);
    float3 sunOrMoonColor = lerp(moonColor, sunDayColor, dayFactor);

    float3 sunFinal = sunOrMoonColor * SUN_INTENSITY * (sunDisk + sunHalo * haloStrength);

    return float4(sky + sunFinal, 1.0);
}