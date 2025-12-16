#version 440 core

const vec3 WORLD_UP = vec3(0, 1, 0);
const int SEGMENTS = 4;

layout (points) in;
layout (triangle_strip, max_vertices = 24) out; // 3 liście × 2 quady × 4 vert

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures;
    vec4 viewDistance;
    vec4 shadowVariables;
    vec4 fogData;
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 fparams; // x -leafScale
    ivec4 atlasParams; // x - atlasSize, y - atlasIndex
} leafParams;

out GS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 color;
} gs_out;

in VS_OUT
{
    vec3 worldPosition;
} gs_in[];

// Emituje jeden quad billboardu
void EmitBillboardVertex(vec3 pos, vec3 normal, vec2 uv)
{
    gl_Position = perFrame.projectionViewMatrix * vec4(pos, 1.0);
    gs_out.texCoord = uv;
    gs_out.worldPos  = vec4(pos, 1.0);
    gs_out.normal    = normalize(normal);
    gs_out.color     = vec3(1, 1, 1);
    EmitVertex();
}

// Funkcja losowa dla offsetów
float Rand(float seed)
{
    return fract(sin(seed) * 43758.5453);
}

void main()
{
    const float leafScale = leafParams.fparams.x;
    vec3 center = gl_in[0].gl_Position.xyz;

    // kierunek do kamery w XZ
    vec3 toCameraXZ = perFrame.cameraPosition - center;
    toCameraXZ.y = 0.0;
    toCameraXZ = normalize(toCameraXZ);

    vec3 right = normalize(cross(WORLD_UP, toCameraXZ));

    // Generujemy 3 liście per punkt (kępka)
    for(int i = 0; i < 3; ++i)
    {
        float seed = float(i) + center.x + center.z;

        // losowy kierunek liścia w poziomie
        float angleRandom = Rand(seed) * 6.283185; // 0..2PI
        vec3 dirRandom = normalize(vec3(cos(angleRandom), 0.0, sin(angleRandom)));

        // blend losowego kierunku z kierunkiem do kamery
        vec3 leafDir = normalize(mix(dirRandom, toCameraXZ, 0.25)); // 0.25 = siła blendu

        // losowy offset wzdłuż kierunku i pionowo
        float radius = 0.1 + 0.1 * Rand(seed + 1.0);
        float yOff   = 0.05 * Rand(seed + 2.0);
        vec3 offset  = leafDir * radius + vec3(0, yOff, 0);
        offset *= leafScale;

        vec3 leafCenter = center + offset;

        // losowy tilt liścia
        float tiltAngle = radians(20.0 + 20.0 * Rand(seed + 3.0));
        vec3 upTilted = normalize(WORLD_UP * cos(tiltAngle) + leafDir * sin(tiltAngle));

        // losowy obrót liścia wokół własnej osi
        float rot = Rand(seed) * 6.283185; // 0..2PI

        // Cross-billboards (2 quady per liść)
        for(int j = 0; j < 2; ++j)
        {
            float qAngle = rot + j * 3.14159/2.0;
            float c = cos(qAngle);
            float s = sin(qAngle);

            vec3 r = (right * c + upTilted * s) * leafScale;
            vec3 u = (-right * s + upTilted * c) * leafScale;

            // przesunięcie dolnej krawędzi do punktu VS
            vec3 baseCenter = leafCenter + upTilted * (0.5 * leafScale);

            // emit 4 wierzchołków quad
            EmitBillboardVertex(baseCenter - r - u, upTilted, vec2(0,0));
            EmitBillboardVertex(baseCenter + r - u, upTilted, vec2(1,0));
            EmitBillboardVertex(baseCenter - r + u, upTilted, vec2(0,1));
            EmitBillboardVertex(baseCenter + r + u, upTilted, vec2(1,1));

            EndPrimitive();
        }
    }
}
