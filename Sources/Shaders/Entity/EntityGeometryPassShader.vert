#version 400 core
const int MAX_BONES = 100;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;
//layout (location = 4) in mat4 TransformationMatrixes;

uniform float UseBoneTransform;
uniform mat4 BonesTransforms[MAX_BONES];

uniform mat4 TransformationMatrix ;
uniform mat4 ProjectionMatrix ;
uniform mat4 ViewMatrix ;

uniform mat4 ToShadowMapSpace;
uniform vec3 ShadowVariables;

uniform float IsUseNormalMap;
uniform float IsUseFakeLighting;
uniform float IsInstancedRender;

uniform vec4 ClipPlane;

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
    TexCoord0      = TexCoord;
    Normal0        = worldData.worldNormal.xyz;
    WorldPos0      = worldData.worldPosition;

	if( IsUseNormalMap > .5f)
	{
		PassTangent = (TransformationMatrix * vec4(Tangent, 0.0)).xyz;
		UseNormalMap = 1.f;
	}
	else
	{
		UseNormalMap = 0.f;
		PassTangent = vec3(.0f) ;
	}

	if (IsUseFakeLighting > .5f)
    {
        Normal0 = vec3(.0f , 1.f, .0f);
		FakeLight = 1.0f;
    }
	else
	{
		FakeLight = 0.f;
	}
	
	Distance = length(modelViewPosition.xyz);

	UseShadows = ShadowVariables.x;

	if (ShadowVariables.x > 0.5f)
	{
		ShadowMapSize = ShadowVariables.z;

		float shadow_distance			= ShadowVariables.y;
		const float transition_distance = 10.f;

		float distance_to_cam   = Distance;
		ShadowCoords			= ToShadowMapSpace * WorldPos0; 
		distance_to_cam			= distance_to_cam - (shadow_distance - transition_distance);
		distance_to_cam			= distance_to_cam / shadow_distance;
		ShadowCoords.w			= clamp(1.f - distance_to_cam, 0.f, 1.f);
	}
}