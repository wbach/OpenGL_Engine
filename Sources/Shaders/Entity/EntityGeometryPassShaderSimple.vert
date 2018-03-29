#version 330
const int MAX_BONES = 100;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in mat4 TransformationMatrixes;
layout (location = 8) in ivec4 BoneIDs;
layout (location = 9) in vec4 Weights;

uniform float UseBoneTransform;
uniform mat4 Bones[MAX_BONES];

uniform mat4 TransformationMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform float numberOfRows;
uniform vec2 textureOffset;

out vec2 TexCoord0;
out vec3 Normal0;
out vec4 WorldPos0;

//Fog
out float Distance;

void main()
{
	mat4 bone_transform = mat4(1.f);
	if(UseBoneTransform > .5f)
	{
		bone_transform  = Bones[BoneIDs[0]] * Weights[0];
		bone_transform += Bones[BoneIDs[1]] * Weights[1];
		bone_transform += Bones[BoneIDs[2]] * Weights[2];
		bone_transform += Bones[BoneIDs[3]] * Weights[3];
	}	
	mat4 transform_matrix = IsInstancedRender > 0.5f ? TransformationMatrixes : TransformationMatrix;
	vec4 world_position = transform_matrix * bone_transform * vec4(Position, 1.f); 

	vec4 model_view_position  = ViewMatrix * world_position;
	gl_ClipDistance[0] = dot(world_position, ClipPlane);

    gl_Position    = ProjectionMatrix * model_view_position;
    TexCoord0      = TexCoord / numberOfRows + textureOffset;
    WorldPos0      = world_position;

	if (IsUseFakeLighting > .5f)
    {
        Normal0 = vec3(.0f ,1.f, .0f);
    }
    else
    {
    	Normal0 = (TransformationMatrix * bone_transform * vec4(Normal, 0.0)).xyz;
    }

	Distance = length(model_view_position.xyz);
}