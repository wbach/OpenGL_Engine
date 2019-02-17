#version 440
const int MAX_BONES = 25;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;

layout (std140, align=16, binding=0) uniform PerApp
{
    float useTextures;
    float viewDistance;
    vec3 shadowVariables;
    vec4 clipPlane;
} perApp;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, align=16, binding=2) uniform PerObjectConstants
{
    float useBoneTransform;
    vec2 textureOffset;
} perObjectConstants;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

layout (std140, binding=4) uniform PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];
} perPoseUpdate;

out VS_OUT
{
    vec2 textureOffset;
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
} vs_out;

struct VertexWorldData
{
    vec4 worldPosition;
    vec4 worldNormal;
};

bool Is(float v)
{
    return v > 0.5f;
}

VertexWorldData caluclateWorldData()
{
    VertexWorldData result;
    result.worldPosition = vec4(0.0);
    result.worldNormal   = vec4(0.0);

    if(!Is(perObjectConstants.useBoneTransform))
    {
        result.worldPosition = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
        result.worldNormal = perObjectUpdate.transformationMatrix * vec4(Normal, 0.0);
        return result;
    }

    for(int i=0; i < MAX_WEIGHTS; i++)
    {
        mat4 boneTransform = perPoseUpdate.bonesTransforms[BoneIds[i]];
        vec4 posePosition = boneTransform * vec4(Position, 1.0f);
        result.worldPosition += posePosition * Weights[i];

        vec4 worldNormal = boneTransform * vec4(Normal, 0.0f);
        result.worldNormal += worldNormal * Weights[i];
    }

    result.worldPosition = perObjectUpdate.transformationMatrix * result.worldPosition;
    result.worldNormal = perObjectUpdate.transformationMatrix * result.worldNormal;
    return result;
}

mat3 CreateTBNMatrix(vec3 normal)
{
    vec3 tangent = normalize((perObjectUpdate.transformationMatrix * vec4(Tangent, 0.0)).xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 binormal = cross(tangent, normal);
    return  mat3(tangent, binormal, normal);
}

void main()
{
    VertexWorldData worldData = caluclateWorldData();

    vs_out.texCoord      = TexCoord;
    vs_out.worldPos      = worldData.worldPosition;
    vs_out.textureOffset = perObjectConstants.textureOffset;
    vs_out.normal        = worldData.worldNormal.xyz;
    vs_out.tbn           = CreateTBNMatrix(worldData.worldNormal.xyz);
    gl_Position = perFrame.projectionViewMatrix * worldData.worldPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, perApp.clipPlane);
}