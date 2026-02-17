#version 440 core
#extension GL_GOOGLE_include_directive : enable
#include "../Common/PerFrameBuffer.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, align=16, binding=8) uniform WaterTileMeshBuffer
{
    vec4 waterColor;
    vec4 tilePosAndScale;
    vec4 params; // x - deltaTime, y - waveSpeed, z - tiledValue, w - isSimpleRender
    vec4 waveParams;
    vec4 projParams;
    vec4 waterDepthVisibility; // x - maxVisibleDepth, y = scaleDepth
} waterTileMeshBuffer;

in VS_OUT
{
    vec4 worldPos;
    vec2 texCoord;
    vec3 normal;
    vec4 clipSpace;
} gs_in[];

out GS_OUT
{
    vec4 worldPos;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec4 clipSpace;
} gs_out;


float waveHeight(vec2 worldPos, float time, float amplitude)
{
    vec2 centerPos = waterTileMeshBuffer.tilePosAndScale.xy;
    vec2 tileScale = waterTileMeshBuffer.tilePosAndScale.zw;

    vec2 d = abs(worldPos - centerPos);

    // połowa szerokości i wysokości
    float halfX = tileScale.x * 0.5;
    float halfY = tileScale.y * 0.5;

    // dystans Chebysheva dopasowany do prostokąta
    float dist = max(d.x / halfX, d.y / halfY);

    // fade-out przy brzegu (0–1 → wewn.:0, krawedz:1)
    //float edgeFactor = 1.0 - smoothstep(0.85, 1.0, dist);

    // falowanie
    float waveFreq = waterTileMeshBuffer.waveParams.y;
    vec2 p = worldPos * waveFreq;

    float wave1 = sin(p.x * 0.5 + time * 1.5) * 0.4;
    float wave2 = cos(p.y * 0.3 + time * 1.0) * 0.3;
    float wave3 = sin((p.x + p.y) * 0.4 + time * 1.2) * 0.2;
    float wave4 = cos((p.x - p.y) * 0.6 + time * 0.8) * 0.15;

    return amplitude * (wave1 + wave2 + wave3 + wave4);
}

vec3 calculateVertexNormal(vec3 worldPos, float time, float amplitude)
{
    vec2 tileScale = waterTileMeshBuffer.tilePosAndScale.zw;
    float gridResolution = waterTileMeshBuffer.waveParams.z;

    float epsX = tileScale.x / gridResolution;
    float epsZ = tileScale.y / gridResolution;

    // próbki wysokości
    float hL = waveHeight(worldPos.xz - vec2(epsX, 0.0), time, amplitude);
    float hR = waveHeight(worldPos.xz + vec2(epsX, 0.0), time, amplitude);
    float hD = waveHeight(worldPos.xz - vec2(0.0, epsZ), time, amplitude);
    float hU = waveHeight(worldPos.xz + vec2(0.0, epsZ), time, amplitude);

    // wektory różniczkowe
    vec3 dx = vec3(2.0 * epsX, hR - hL, 0.0);
    vec3 dz = vec3(0.0, hU - hD, 2.0 * epsZ);

    return normalize(cross(dz, dx));
}

void calculateTangentBitangentPerVertex(
    vec3 pos, vec2 uv,
    float time, float amplitude,
    out vec3 tangent, out vec3 bitangent)
{
    vec2 tileScale = waterTileMeshBuffer.tilePosAndScale.zw;
    float gridResolution = waterTileMeshBuffer.waveParams.z;
    float epsX = tileScale.x / float(gridResolution);
    float epsZ = tileScale.y / float(gridResolution);

    vec3 posU = pos + vec3(epsX, waveHeight(pos.xz + vec2(epsX, 0), time, amplitude) - waveHeight(pos.xz, time, amplitude), 0.0);
    vec3 posV = pos + vec3(0.0, waveHeight(pos.xz + vec2(0, epsZ), time, amplitude) - waveHeight(pos.xz, time, amplitude), epsZ);

    tangent = normalize(posU - pos);
    bitangent = normalize(posV - pos);
}

void main()
{
    for (int i = 0; i < gl_in.length(); ++i)
    {
        vec4 worldPosition = gs_in[i].worldPos;
        worldPosition.y += waveHeight(worldPosition.xz, waterTileMeshBuffer.params.y, waterTileMeshBuffer.waveParams.x);

        gs_out.worldPos  =  worldPosition;
        gs_out.texCoord  = gs_in[i].texCoord;
        gs_out.normal = calculateVertexNormal(worldPosition.xyz, waterTileMeshBuffer.params.y, waterTileMeshBuffer.waveParams.x);

        vec3 tangent, bitangent;
        calculateTangentBitangentPerVertex(
            worldPosition.xyz,
            gs_in[i].texCoord,
            waterTileMeshBuffer.params.y,
            waterTileMeshBuffer.waveParams.x,
            tangent,
            bitangent
        );

        gs_out.tangent = tangent;
        gs_out.bitangent = bitangent;

        gs_out.clipSpace = perFrame.projectionViewMatrix * worldPosition;
        gl_Position = perFrame.projectionViewMatrix * worldPosition;

        EmitVertex();
    }
    EndPrimitive();
}
