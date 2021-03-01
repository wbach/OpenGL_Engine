#version 440 core
const int MAX_BONES = 512;
const int MAX_INSTANCES = 512;
const int MAX_WEIGHTS = 4;

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec4 Weights;
layout (location = 5) in ivec4 BoneIds;
//

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
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
    mat4 meshMatrix;
} perObjectUpdate;

layout (std140, align=16, binding=4) uniform PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];
} perPoseUpdate;

layout (std140, align=16, binding=5) uniform PerInstance
{
    mat4 transformationMatrix[MAX_INSTANCES];
} perInstance;

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
    return perInstance.transformationMatrix[gl_InstanceID] * perObjectUpdate.meshMatrix * vec4(Position, 1.f);
}

void main(void)
{
    gl_Position =  perFrame.projectionViewMatrix * caluclateWorldPosition();
    vs_out.texCoord = TexCoord;
    vs_out.textureOffset = perObjectConstants.textureOffset;
}