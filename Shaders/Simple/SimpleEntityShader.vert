#version 330 core
const int MAX_BONES = 100;

layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;   
layout (location = 4) in mat4 TransformationMatrixes;
layout (location = 8) in ivec4 BoneIDs;
layout (location = 9) in vec4 Weights;

uniform float UseBoneTransform;
uniform mat4 Bones[MAX_BONES];

out vec2 textureCoords ;

uniform mat4 TransformationMatrix ;
uniform mat4 ProjectionMatrix ;
uniform mat4 ViewMatrix ;
uniform float IsInstancedRender ;

void main(void)
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
	gl_Position =  ProjectionMatrix * ViewMatrix * transform_matrix *bone_transform* vec4(Position, 1.0);
    textureCoords = TexCoord;
}