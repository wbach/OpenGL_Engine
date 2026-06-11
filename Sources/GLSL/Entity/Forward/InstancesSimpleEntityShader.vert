#version 440
#extension GL_GOOGLE_include_directive : enable
#include "../../Common/PerApp.glsl"
#include "../../Common/PerFrameBuffer.glsl"

const int MAX_INSTANCES = 512;
const int MAX_BONES = 512;
const int MAX_WEIGHTS = 4;

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec4 Weights;
layout (location = 5) in ivec4 BoneIds;

layout (std140, align=16, binding=2) uniform PerObjectConstants
{
    float useBoneTransform;
    vec2 textureOffset;
} perObjectConstants;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 meshMatrix;
} perObjectUpdate;

layout (std140, align=16, binding=5) uniform PerInstance
{
    mat4 transformationMatrix[MAX_INSTANCES];
} perInstance;

layout (std140, align=16, binding=4) uniform PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];
} perPoseUpdate;

out VS_OUT
{
    vec2 texCoord;
    vec2 textureOffset;
    vec3 normal;
    float outOfViewRange;
    float visibility;
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
    result.worldPosition = perInstance.transformationMatrix[gl_InstanceID] * perObjectUpdate.meshMatrix * vec4(Position, 1.f);
    result.worldNormal = perInstance.transformationMatrix[gl_InstanceID] * perObjectUpdate.meshMatrix * vec4(Normal, 0.0);

    return result;
}

void main()
{
    VertexWorldData worldData = caluclateWorldData();

    vs_out.normal        = normalize(worldData.worldNormal.xyz);
    vs_out.texCoord      = TexCoord;
    vs_out.textureOffset = vec2(0);//perObjectConstants.textureOffset;
    vs_out.outOfViewRange = 0.f; //length(modelViewPosition.xyz) > perApp.viewDistance ? 1.f : 0.f;
    gl_Position = perFrame.projectionViewMatrix * worldData.worldPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, perFrame.clipPlane);

    float distance = length(perFrame.cameraPosition - worldData.worldPosition.xyz);
    float visibility = exp(-pow((distance*( ( 3.f / perApp.viewDistance.x))), perApp.fogData.w));
    vs_out.visibility = clamp(visibility, 0.f, 1.f);
}