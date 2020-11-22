#version 440
const int MAX_BONES = 512;
const int MAX_WEIGHTS = 4;

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec4 Weights;
layout (location = 5) in ivec4 BoneIds;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    float viewDistance;
    vec3 shadowVariables;
} perApp;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
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

layout (std140, align=16, binding=4) uniform PerPoseUpdate
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

    vs_out.texCoord      = TexCoord;
    vs_out.textureOffset = perObjectConstants.textureOffset;
    vs_out.outOfViewRange = 0.f; //length(modelViewPosition.xyz) > perApp.viewDistance ? 1.f : 0.f;
    gl_Position = perFrame.projectionViewMatrix * worldData.worldPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, perFrame.clipPlane);
}