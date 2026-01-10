#version 450 core

// Twoje istniejące uniformy - bez zmian
layout (std140, align=16, binding=1) uniform PerFrame {
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

layout(std140, align=16, binding = 4) uniform LeafParams {
    vec4 wind; 
    vec4 fparams; 
    ivec4 atlasParams; 
    float time;
} leafParams;

struct Leaf {
    vec4 posSize;    // xyz: worldPosition, w: sizeRandomness
    vec4 dirUnused;  // xyz: worldDirection, w: unused
    vec4 colorTex;   // xyz: colorRandomness, w: textureIndex (jako float)
};

layout(std430, binding = 5) buffer LeafData {
    Leaf leaves[];
};

out VS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 colorRandomness;
    flat int textureIndex;
} vs_out;


// out VS_OUT_TO_FS {
//     vec2 texCoord;
//     vec4 worldPos;
//     vec3 normal;
//     vec3 colorRandomness;
//     flat int textureIndex;
// } vs_out;

// Pomocnicza funkcja do wiatru (przeniesiona z Twojego GS)
vec3 ComputeWindBend(float time, float leafPhase, vec3 windDir, float windStrength, float t, vec3 outwardDir) {
    vec3 sideDir = normalize(cross(windDir, outwardDir)); 
    float flutter = sin(time * 8.0 + leafPhase * 3.0) * windStrength * 0.2;
    return windDir * windStrength * (t * t) + sideDir * flutter * t;
}

void main() {
    // 1. Wyznaczamy ID liścia i ID wierzchołka (0-5 dla 2 trójkątów)
    int leafID = gl_VertexID / 6;
    int cornerID = gl_VertexID % 6;

    // 2. Pobieramy dane liścia z SSBO
    Leaf l = leaves[leafID];
    vec3 localPosData = l.posSize.xyz; // Teraz traktujemy to jako pozycję lokalną
    float sizeRandomness = l.posSize.w;
    vec3 localDirection = l.dirUnused.xyz;
    vec3 colorRandomness = l.colorTex.xyz;
    int textureIndex = int(l.colorTex.w);

    // 3. Logika generowania geometrii (Lokalna)
    float leafPhase = dot(localPosData, vec3(0.13, 0.27, 0.19));
    vec3 windDir = normalize(leafParams.wind.xyz);
    float windStrength = leafParams.wind.w;
    float sway = sin(leafParams.time * 1.5 + leafPhase) * windStrength;
    vec3 windOffset = windDir * sway;

    float scale = leafParams.fparams.x * sizeRandomness;
    float leafOffset = leafParams.fparams.y;
    float bendAmount = leafParams.fparams.z;

    vec3 outwardDir = normalize(localDirection);
    vec3 leafBase = localPosData + outwardDir * leafOffset + windOffset * 0.1;

    vec3 upRef = vec3(0.0, 1.0, 0.0); 
    vec3 right = normalize(cross(outwardDir, upRef));
    if(length(right) < 0.01) right = normalize(cross(outwardDir, vec3(1,0,0)));
    right *= scale;

    vec3 up = outwardDir * scale;
    vec3 bend = vec3(0.0, -1.0, 0.0) * bendAmount; 
    vec3 windTop = ComputeWindBend(leafParams.time, leafPhase, windDir, windStrength, 1.0, outwardDir);

    // 4. Mapowanie wierzchołków dla trójkątów (Lokalne)
    vec3 v0 = leafBase - right * 0.5;
    vec3 v1 = leafBase + right * 0.5;
    vec3 v2 = leafBase + right * 0.5 + up + bend + windTop;
    vec3 v3 = leafBase - right * 0.5 + up + bend + windTop;

    vec3 verts[6] = vec3[]( v0, v1, v2, v0, v2, v3 );
    
    vec2 uv0 = vec2(0,0); vec2 uv1 = vec2(1,0); vec2 uv2 = vec2(1,1); vec2 uv3 = vec2(0,1);
    vec2 uvs[6]   = vec2[]( uv0, uv1, uv2, uv0, uv2, uv3 );

    // 5. Transformacja do świata, Normalna i wyjście
    // Aplikujemy macierz modelu drzewa do lokalnych wierzchołków
    vec4 worldPos = perObjectUpdate.transformationMatrix * vec4(verts[cornerID], 1.0);
    
    vec3 bentUp = up + windTop;
    vec3 localNormal = normalize(cross(right, bentUp));
    
    // Transformujemy normalną do świata (mat3 usuwa translację z macierzy)
    vs_out.normal = normalize(mat3(perObjectUpdate.transformationMatrix) * localNormal);
    
    vs_out.texCoord = uvs[cornerID];
    vs_out.worldPos = worldPos;
    vs_out.colorRandomness = colorRandomness;
    vs_out.textureIndex = textureIndex;

    // Ostateczna pozycja na ekranie
    gl_Position = perFrame.projectionViewMatrix * worldPos;
}