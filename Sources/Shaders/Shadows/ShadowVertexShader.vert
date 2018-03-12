#version 330 core
const int MAX_BONES = 100;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;

uniform float UseBoneTransform;
uniform mat4 BonesTransforms[MAX_BONES];

out vec2 textureCoords;

uniform mat4 TransformationMatrix ;
uniform mat4 ProjectionMatrix ;
uniform mat4 ViewMatrix ;
uniform float IsInstancedRender ;

vec4 caluclateWorldPosition()
{
	if(UseBoneTransform < .5f)
		return TransformationMatrix * vec4(Position, 1.f);

	vec4 totalLocalPos = vec4(0.0);

	for(int i = 0; i < MAX_WEIGHTS; i++)
	{
		mat4 boneTransform = BonesTransforms[BoneIds[i]];
		vec4 posePosition = boneTransform * vec4(Position, 1.0f);
		totalLocalPos += posePosition * Weights[i];
	}
	return TransformationMatrix * totalLocalPos;
}

void main(void)
{
	gl_Position =  ProjectionMatrix * caluclateWorldPosition();
    textureCoords = TexCoord;
}