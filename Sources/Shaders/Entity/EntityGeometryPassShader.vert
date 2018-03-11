#version 400 core
const int MAX_BONES = 100;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 joinIds;
//layout (location = 4) in mat4 TransformationMatrixes;

uniform float UseBoneTransform;
uniform mat4 joinTransforms[MAX_BONES];

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

void main()
{     
	vec4 world_position;
	mat4 bone_transform = mat4(1.f);

	if(UseBoneTransform > .5f)
	{
		vec4 totalLocalPos = vec4(0.0);
		vec4 totalNormal = vec4(0.0);

		for(int i=0; i<1; i++)
		{
			mat4 jointTransform = joinTransforms[joinIds[i]];
			vec4 posePosition = jointTransform * vec4(Position, 1.0f);
			totalLocalPos += posePosition * Weights[i];
			
			vec4 worldNormal = jointTransform * vec4(Normal, 0.0f);
			totalNormal += worldNormal * Weights[i];
		}
		world_position  = TransformationMatrix * totalLocalPos;
		//bone_transform += joinTransforms[BoneIDs[3]] * Weights[3];
	}
	else
	{
		mat4 transform_matrix = TransformationMatrix ;//IsInstancedRender > 0.5f ? TransformationMatrixes : TransformationMatrix;
		world_position = transform_matrix * bone_transform * vec4(Position, 1.f);
	}

	//gl_Position    = ProjectionMatrix * ViewMatrix * world_position;
	//return;

	vec4 model_view_position  = ViewMatrix * world_position;
	gl_ClipDistance[0] = dot(world_position, ClipPlane) ;

    gl_Position    = ProjectionMatrix * model_view_position; 
    TexCoord0      = TexCoord;
    Normal0        = (TransformationMatrix * bone_transform * vec4(Normal, 0.0)).xyz;
    WorldPos0      = world_position;

	if( IsUseNormalMap > .5f)
	{
		PassTangent = (TransformationMatrix * bone_transform * vec4(Tangent, 0.0)).xyz;
		UseNormalMap = 1.f;
	}
	else
	{
		UseNormalMap = 0.f;
		PassTangent = vec3(.0f) ;
	}

	if (IsUseFakeLighting > .5f)
    {
        Normal0 = vec3(.0f ,1.f, .0f) ;
		FakeLight = 1.0f;
    }
	else
	{
		FakeLight = 0.f;
	}
	
	Distance = length(model_view_position.xyz);

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