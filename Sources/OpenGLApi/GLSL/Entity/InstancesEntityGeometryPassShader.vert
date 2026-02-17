#version 440 core
#extension GL_GOOGLE_include_directive : enable
#include "../Common/PerApp.glsl"
#include "../Common/PerFrameBuffer.glsl"

const int MAX_INSTANCES = 512;
const int MAX_BONES = 512;
const int MAX_SHADOW_MAP_CASADES = 4;
const int MAX_WEIGHTS = 4;

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

layout (std140, align=16, binding=4) uniform PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];
} perPoseUpdate;

layout (std140, align=16, binding=5) uniform PerInstance
{
    mat4 transformationMatrix[MAX_INSTANCES];
} perInstance;

layout (std140,binding=7) uniform ShadowsBuffer
{
    mat4 directionalLightSpace[MAX_SHADOW_MAP_CASADES];
    vec4 cascadesDistance;
    float cascadesSize;
} shadowsBuffer;

out VS_OUT
{
    vec2 textureOffset;
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
    float depth;
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

mat3 CreateTBNMatrix(vec3 normal)
{
    vec3 tangent  = normalize((perInstance.transformationMatrix[gl_InstanceID] * perObjectUpdate.meshMatrix * vec4(Tangent, 0.0)).xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 binormal = cross(normal, tangent);
    return mat3(tangent, binormal, normal);
}

bool NormalMaping(vec3 worldPos)
{
    float dist = length(perFrame.cameraPosition - worldPos);
    return Is(perApp.useTextures.y) && (dist < perApp.viewDistance.y);
}

void main()
{
    VertexWorldData worldData = caluclateWorldData();

    float depth = length(perFrame.cameraPosition - worldData.worldPosition.xyz);
    const float farPlane = perFrame.projection.y;
    float linearDepth  = depth / farPlane;

    vs_out.texCoord      = TexCoord;
    vs_out.worldPos      = worldData.worldPosition;
    vs_out.textureOffset = vec2(0);//perObjectConstants.textureOffset;
    vs_out.normal        = normalize(worldData.worldNormal.xyz);
    vs_out.depth         = depth;
    if (NormalMaping(vs_out.worldPos.xyz))
    {
        vs_out.tbn = CreateTBNMatrix(vs_out.normal);
    }

    gl_Position = perFrame.projectionViewMatrix * worldData.worldPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, perFrame.clipPlane);
}