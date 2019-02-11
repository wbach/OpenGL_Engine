#version 440
const int MAX_BONES = 100;
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

layout (std140, binding=1) uniform PerResize
{
    mat4 projectionMatrix;
} perResize;

layout (std140,binding=2) uniform PerFrame
{
    mat4 viewMatrix;
    mat4 toShadowMapSpace;
} perFrame;

layout (std140, align=16, binding=3) uniform PerObjectConstants
{
    float useBoneTransform;
    vec2 textureOffset;
} perObjectConstants;

layout (std140, binding=4) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

layout (std140, binding=5) uniform PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];
} perPoseUpdate;

out VS_OUT
{
    vec2 texCoord;
    vec2 textureOffset;
    float outOfViewRange;
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

void main()
{
    VertexWorldData worldData = caluclateWorldData();

    vec4 modelViewPosition = perFrame.viewMatrix * worldData.worldPosition;
    vs_out.texCoord      = TexCoord;
    vs_out.textureOffset = perObjectConstants.textureOffset;
    vs_out.outOfViewRange = length(modelViewPosition.xyz) > perApp.viewDistance ? 1.f : 0.f;
    gl_Position = perResize.projectionMatrix * modelViewPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, perApp.clipPlane);
}