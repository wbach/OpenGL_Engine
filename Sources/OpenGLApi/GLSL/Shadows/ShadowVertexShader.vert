#version 440 core
const int MAX_BONES = 25;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;

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
    gl_Position =  perResize.projectionMatrix * perFrame.viewMatrix * caluclateWorldPosition();
    vs_out.texCoord = TexCoord;
    vs_out.textureOffset = perObjectConstants.textureOffset;
}