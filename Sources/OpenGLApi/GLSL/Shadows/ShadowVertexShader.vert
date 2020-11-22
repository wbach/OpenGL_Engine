#version 440 core
const int MAX_BONES = 512;
const int MAX_WEIGHTS = 4;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
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
} vs_out;

bool Is(float v)
{
    return v > 0.5f;
}

vec4 caluclateWorldPosition()
{
    if(!Is(perObjectConstants.useBoneTransform))
        return perObjectUpdate.transformationMatrix * vec4(Position, 1.f);

    vec4 totalLocalPos = vec4(0.0);

    for(int i = 0; i < MAX_WEIGHTS; i++)
    {
        mat4 boneTransform = perPoseUpdate.bonesTransforms[BoneIds[i]];
        vec4 posePosition = boneTransform * vec4(Position, 1.0f);
        totalLocalPos += posePosition * Weights[i];
    }
    return perObjectUpdate.transformationMatrix * totalLocalPos;
}

void main(void)
{
    gl_Position =  perFrame.projectionViewMatrix * caluclateWorldPosition();
    vs_out.texCoord = TexCoord;
    vs_out.textureOffset = perObjectConstants.textureOffset;
}