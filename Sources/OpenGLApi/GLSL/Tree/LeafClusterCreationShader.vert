#version 450 core

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

// layout(std140, align=16, binding = 4) uniform LeafParams
// {
//     vec4 wind; 
//     vec4 fparams; 
//     ivec4 atlasParams; 
//     float time;
// } leafParams;

struct Leaf
{
    vec4 posSize;    
    vec4 dirUnused;  
    vec4 colorTex;   
};

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


void main() 
{
    int clusterLeafID = gl_VertexID / 6;
    int cornerID = gl_VertexID % 6;
    
    uint globalLeafID = indices[clusterLeafID];

    Leaf l = leaves[globalLeafID];
    vec3 localPosData = l.posSize.xyz;
    float sizeRandomness = l.posSize.w;
    vec3 localDirection = l.dirUnused.xyz;
    vec3 colorRandomness = l.colorTex.xyz;
    int textureIndex = int(l.colorTex.w);

    float scale = sizeRandomness;

    vec3 outwardDir = normalize(localDirection);
    vec3 leafBase = localPosData + outwardDir * 0.1;

    vec3 upRef = vec3(0.0, 1.0, 0.0); 
    vec3 right = normalize(cross(outwardDir, upRef));
    if(length(right) < 0.01) right = normalize(cross(outwardDir, vec3(1,0,0)));
    right *= scale;

    vec3 up = outwardDir * scale;

    vec3 v0 = leafBase - right * 0.5;
    vec3 v1 = leafBase + right * 0.5;
    vec3 v2 = leafBase + right * 0.5 + up;
    vec3 v3 = leafBase - right * 0.5 + up;

    vec3 verts[6] = vec3[]( v0, v1, v2, v0, v2, v3 );
    
    vec2 uv0 = vec2(0,0); vec2 uv1 = vec2(1,0); vec2 uv2 = vec2(1,1); vec2 uv3 = vec2(0,1);
    vec2 uvs[6] = vec2[]( uv0, uv1, uv2, uv0, uv2, uv3 );

    vec4 worldPos = perObjectUpdate.transformationMatrix * vec4(verts[cornerID], 1.0);
    vec3 localNormal = normalize(cross(right, up));
    
    vs_out.normal = normalize(mat3(perObjectUpdate.transformationMatrix) * localNormal);
    vs_out.texCoord = uvs[cornerID];
    vs_out.colorRandomness = colorRandomness;
    vs_out.textureIndex = textureIndex;

    gl_Position = worldPos;
}