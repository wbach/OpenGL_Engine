#version 450 core

struct Leaf
{
    vec4 posSize;
    vec4 dirUnused;
    vec4 colorTex;
};

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 mvp;
} perObjectUpdate;

layout(std430, binding = 5) buffer LeafData
{
    Leaf leaves[];
};

layout(std430, binding = 6) buffer LeafIndices
{
    uint indices[];
};

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 colorRandomness;
    flat int textureIndex;
} vs_out;


/* =========================
   LOCAL WIND CONSTANTS
   ========================= */

const vec3 WIND_DIR = normalize(vec3(0.7, 0.0, 0.3));
const float WIND_STRENGTH = 0.8;

/* =========================
   WIND FUNCTION (time = 0)
   ========================= */

vec3 ComputeWindBend(vec3 outwardDir, float leafPhase)
{
    vec3 sideDir = normalize(cross(WIND_DIR, outwardDir));
    float flutter = sin(leafPhase * 3.0) * WIND_STRENGTH * 0.2;
    return WIND_DIR * WIND_STRENGTH + sideDir * flutter;
}

void main()
{
    int clusterLeafID = gl_VertexID / 6;
    int cornerID = gl_VertexID % 6;

    uint globalLeafID = indices[clusterLeafID];
    Leaf l = leaves[globalLeafID];

    vec3 localPosData = l.posSize.xyz;
    float sizeRandomness = l.posSize.w;
    vec3 localDirection = l.dirUnused.xyz;

    vs_out.colorRandomness = l.colorTex.xyz;
    vs_out.textureIndex = int(l.colorTex.w);

    float scale = sizeRandomness;
    vec3 outwardDir = normalize(localDirection);

    float leafPhase = dot(localPosData, vec3(0.13, 0.27, 0.19));
    vec3 windTop = ComputeWindBend(outwardDir, leafPhase);

    vec3 leafBase = localPosData + outwardDir * 0.1 + windTop * 0.1;

    vec3 upRef = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(outwardDir, upRef));
    if (length(right) < 0.01)
        right = normalize(cross(outwardDir, vec3(1, 0, 0)));

    right *= scale;
    vec3 up = outwardDir * scale;

    vec3 topOffset = up  + windTop;

    vec3 v0 = leafBase - right * 0.5;
    vec3 v1 = leafBase + right * 0.5;
    vec3 v2 = leafBase + right * 0.5 + topOffset;
    vec3 v3 = leafBase - right * 0.5 + topOffset;

    vec3 verts[6] = vec3[](v0, v1, v2, v0, v2, v3);

    vec2 uv0 = vec2(0,0);
    vec2 uv1 = vec2(1,0);
    vec2 uv2 = vec2(1,1);
    vec2 uv3 = vec2(0,1);
    vec2 uvs[6] = vec2[](uv0, uv1, uv2, uv0, uv2, uv3);

    vec4 worldPos = perObjectUpdate.mvp * vec4(verts[cornerID], 1.0);

    vec3 localNormal = normalize(cross(right, topOffset));
    vs_out.normal    = normalize(mat3(perObjectUpdate.mvp) * localNormal);
    vs_out.texCoord  = uvs[cornerID];

    gl_Position = worldPos;
}
