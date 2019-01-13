#version 400 core
const int MAX_BONES = 100;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;

uniform float UseBoneTransform;
uniform mat4 BonesTransforms[MAX_BONES];

uniform mat4 TransformationMatrix ;
uniform mat4 ProjectionMatrix ;
uniform mat4 ViewMatrix ;

uniform vec4 ClipPlane;
uniform uint NumberOfRows;
uniform vec2 TextureOffset;

out vec2 TexCoord0;
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

    if(UseBoneTransform < .5f)
    {
        result.worldPosition = TransformationMatrix * vec4(Position, 1.f);
        result.worldNormal = TransformationMatrix * vec4(Normal, 0.0);
        return result;
    }

    for(int i=0; i < MAX_WEIGHTS; i++)
    {
        mat4 boneTransform = BonesTransforms[BoneIds[i]];
        vec4 posePosition = boneTransform * vec4(Position, 1.0f);
        result.worldPosition += posePosition * Weights[i];

        vec4 worldNormal = boneTransform * vec4(Normal, 0.0f);
        result.worldNormal += worldNormal * Weights[i];
    }

    result.worldPosition = TransformationMatrix * result.worldPosition;
    result.worldNormal = TransformationMatrix * result.worldNormal;
    return result;
}

void main()
{
    VertexWorldData worldData = caluclateWorldData();

    vec4 modelViewPosition  = ViewMatrix * worldData.worldPosition;
    gl_ClipDistance[0] = dot(worldData.worldPosition, ClipPlane);

    gl_Position    = ProjectionMatrix * modelViewPosition; 
    TexCoord0      = (TexCoord / NumberOfRows) + TextureOffset;

    Distance = length(modelViewPosition.xyz);
}