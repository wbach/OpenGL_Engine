#include "EntityShader.h"

void CEntityShader::Init()
{
	SetFiles
	({
		{"Simple/SimpleEntityShader.vert", GL_VERTEX_SHADER},
		{"Simple/SimpleEntityShader.frag", GL_FRAGMENT_SHADER}
	});

	CShaderProgram::Init();
}

void CEntityShader::GetAllUniformLocations()
{
	location_TransformationMatrix = GetUniformLocation("TransformationMatrix");
	location_ProjectionMatrix = GetUniformLocation("ProjectionMatrix");
	location_ViewMatrix = GetUniformLocation("ViewMatrix");

	location_IsInstancedRender = GetUniformLocation("IsInstancedRender");

	//Animations
	location_UseBoneTransform = GetUniformLocation("UseBoneTransform");

	for (int x = 0; x < MAX_BONES; x++)
		location_Bones[x] = GetUniformLocation("BonesTransforms[" + std::to_string(x) + "]");
}

void CEntityShader::BindAttributes()
{
	BindAttribute(0, "Position");
	BindAttribute(1, "TexCoord");
	BindAttribute(2, "Normal");
	BindAttribute(3, "Tangent");
	BindAttribute(4, "Weights");
	BindAttribute(5, "BoneIds");
	//BindAttribute(4, "TransformationMatrixes");
}
void CEntityShader::LoadUseInstancedRendering(const float& use) const
{
	LoadValue(location_IsInstancedRender, use);
}
void CEntityShader::LoadTransformMatrix(const mat4& matrix) const
{
	LoadValue(location_TransformationMatrix, matrix);
}

void CEntityShader::LoadProjectionMatrix(const mat4& matrix) const
{
	LoadValue(location_ProjectionMatrix, matrix);
}

void CEntityShader::LoadViewMatrix(const mat4& matrix) const
{
	LoadValue(location_ViewMatrix, matrix);
}
void CEntityShader::LoadUseBonesTransformation(const float& is) const
{
	LoadValue(location_UseBoneTransform, is);
}
void CEntityShader::LoadBoneTransform(const mat4& transform, uint32 id) const
{
	LoadValue(location_Bones[id], transform);
}
