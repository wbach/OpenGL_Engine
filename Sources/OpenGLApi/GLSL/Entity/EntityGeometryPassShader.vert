#version 420
const int MAX_BONES = 100;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;
//layout (location = 4) in mat4 TransformationMatrixes;

layout (std140, binding = 0) uniform PerApp
{
    bool UseFakeLighting;
    vec3 ShadowVariables;
    vec4 ClipPlane;
} perApp;

layout (std140, binding = 1) uniform PerResize
{
    mat4 ProjectionMatrix;
} perResize;

layout (std140, binding = 2) uniform PerFrame
{
    mat4 ViewMatrix;
    mat4 ToShadowMapSpace;
} perFrame;

layout (std140, binding = 3) uniform PerObject
{
    bool UseBoneTransform;
    bool UseNormalMap;
    mat4 TransformationMatrix;
    uint NumberOfRows;
    vec2 TextureOffset;
    mat4 BonesTransforms[MAX_BONES];
} perObject;

out vec2 TexCoord0;
out vec3 Normal0;
out vec4 WorldPos0;

out vec3 PassTangent;
out float UseNormalMap;
out float FakeLight;

out vec4 ShadowCoords;
out float UseShadows;
out float ShadowMapSize;

//Fog
out float Distance;

struct VertexWorldData
{
    vec4 worldPosition;
    vec4 worldNormal;
};

VertexWorldData caluclateWorldData()
{
    VertexWorldData result;
    result.worldPosition = vec4(0.0);
    result.worldNormal = vec4(0.0);

    if(!perObject.UseBoneTransform)
    {
        result.worldPosition = perObject.TransformationMatrix * vec4(Position, 1.f);
        result.worldNormal = perObject.TransformationMatrix * vec4(Normal, 0.0);
        return result;
    }

    for(int i=0; i < MAX_WEIGHTS; i++)
    {
        mat4 boneTransform = perObject.BonesTransforms[BoneIds[i]];
        vec4 posePosition = boneTransform * vec4(Position, 1.0f);
        result.worldPosition += posePosition * Weights[i];

        vec4 worldNormal = boneTransform * vec4(Normal, 0.0f);
        result.worldNormal += worldNormal * Weights[i];
    }

    result.worldPosition = perObject.TransformationMatrix * result.worldPosition;
    result.worldNormal = perObject.TransformationMatrix * result.worldNormal;
    return result;
}

void CalculateShadowVariables()
{
    UseShadows = perApp.ShadowVariables.x;
    if (UseShadows > 0.5f)
    {
        ShadowMapSize = perApp.ShadowVariables.z;

        float shadow_distance           = perApp.ShadowVariables.y;
        const float transition_distance = 10.f;

        float distance_to_cam   = Distance;
        ShadowCoords            = perFrame.ToShadowMapSpace * WorldPos0;
        distance_to_cam         = distance_to_cam - (shadow_distance - transition_distance);
        distance_to_cam         = distance_to_cam / shadow_distance;
        ShadowCoords.w          = clamp(1.f - distance_to_cam, 0.f, 1.f);
    }
}

void CalculateTangents()
{
    if(perObject.UseNormalMap)
    {
        PassTangent = (perObject.TransformationMatrix * vec4(Tangent, 0.0)).xyz;
        UseNormalMap = 1.f;
    }
    else
    {
        UseNormalMap = 0.f;
        PassTangent = vec3(.0f) ;
    }
}

void main()
{
    VertexWorldData worldData = caluclateWorldData();

    vec4 modelViewPosition  = perFrame.ViewMatrix * worldData.worldPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, perApp.ClipPlane);

    gl_Position    = perResize.ProjectionMatrix * modelViewPosition;
    TexCoord0      = (TexCoord / perObject.NumberOfRows) + perObject.TextureOffset;
    WorldPos0      = worldData.worldPosition;

    CalculateTangents();

    if (perApp.UseFakeLighting)
    {
        Normal0 = vec3(.0f , 1.f, .0f);
        FakeLight = 1.0f;
    }
    else
    {
        Normal0   = worldData.worldNormal.xyz;
        FakeLight = 0.f;
    }

    Distance = length(modelViewPosition.xyz);
    CalculateShadowVariables();
}