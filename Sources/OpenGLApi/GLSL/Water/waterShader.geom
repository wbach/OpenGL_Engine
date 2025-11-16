#version 440 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140, align=16, binding=8) uniform WaterTileMeshBuffer
{
    vec4 waterColor;
    vec4 params; // x - moveFactor, y - waveFactor, z - tiledValue, w - isSimpleRender
    vec4 waveParams;
} waterTileMeshBuffer;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 clipSpace;
} gs_in[];

out GS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 clipSpace;
} gs_out;


float waveHeight(vec2 pos, float time, float amplitude)
{
    // skaluje pos, żeby uniknąć przeskoków przy większych tile
    float waveFreq = waterTileMeshBuffer.waveParams.y;
    vec2 p = pos * waveFreq;  

    // fala wzdłuż X
    float wave1 = sin(p.x * 0.5 + time * 1.5) * 0.4;

    // fala wzdłuż Y
    float wave2 = cos(p.y * 0.3 + time * 1.0) * 0.3;

    // poprzeczna fala po skosie x+y
    float wave3 = sin((p.x + p.y) * 0.4 + time * 1.2) * 0.2;

    // poprzeczna fala po przeciwnym skosie x-y
    float wave4 = cos((p.x - p.y) * 0.6 + time * 0.8) * 0.15;

    // suma fal i skalowanie globalne amplitudą
    return amplitude * (wave1 + wave2 + wave3 + wave4);
}

// Gerstner Waves
// float waveHeight(vec2 pos, float time, float amplitude)
// {
//     // Skaluje współrzędne, aby argumenty sin/cos były w rozsądnym zakresie
//     vec2 p = pos * 0.5;

//     // Fala 1 – kierunek X
//     float wave1 = sin(dot(p, vec2(1.0, 0.0)) * 2.0 + time * 1.5) * 0.4;

//     // Fala 2 – kierunek Y
//     float wave2 = cos(dot(p, vec2(0.0, 1.0)) * 3.0 + time * 1.0) * 0.3;

//     // Fala 3 – diagonalna x+y
//     float wave3 = sin(dot(p, vec2(1.0, 1.0)) * 1.5 + time * 1.2) * 0.2;

//     // Fala 4 – diagonalna x-y
//     float wave4 = cos(dot(p, vec2(1.0, -1.0)) * 2.5 + time * 0.8) * 0.15;

//     // Suma fal i skalowanie globalnym parametrem amplitudy
//     return amplitude * (wave1 + wave2 + wave3 + wave4);
// }

vec3 calculateVertexNormal(vec3 worldPos, float time, float amplitude)
{
    float eps = 0.01;

    // drobne przesunięcia w przestrzeni X/Z
    float hL = waveHeight(worldPos.xz - vec2(eps,0), time, amplitude);
    float hR = waveHeight(worldPos.xz + vec2(eps,0), time, amplitude);
    float hD = waveHeight(worldPos.xz - vec2(0,eps), time, amplitude);
    float hU = waveHeight(worldPos.xz + vec2(0,eps), time, amplitude);

    vec3 dx = vec3(2.0*eps, hR - hL, 0.0);
    vec3 dz = vec3(0.0, hU - hD, 2.0*eps);

    return normalize(cross(dz, dx));
}

void main()
{
    for (int i = 0; i < gl_in.length(); ++i)
    {
        vec4 worldPosition = gs_in[i].worldPos;
        worldPosition.y += waveHeight(worldPosition.xz, waterTileMeshBuffer.params.y, waterTileMeshBuffer.waveParams.x);

        gs_out.texCoord  = gs_in[i].texCoord;
        //gs_out.normal    = gs_in[i].normal;
        gs_out.normal = calculateVertexNormal(worldPosition.xyz, waterTileMeshBuffer.params.y, waterTileMeshBuffer.waveParams.x);
        gs_out.worldPos  = worldPosition;
        gs_out.clipSpace = perFrame.projectionViewMatrix * worldPosition; 
        gl_Position = perFrame.projectionViewMatrix * worldPosition;

        // gs_out.texCoord         = gs_in[i].texCoord;
        // gs_out.normal           = gs_in[i].normal;
        // gs_out.worldPos         = gs_in[i].worldPos;
        // gs_out.clipSpace         = gs_in[i].clipSpace;
        // gl_Position             = gl_in[i].gl_Position;

        EmitVertex();
    }
    EndPrimitive();
}
