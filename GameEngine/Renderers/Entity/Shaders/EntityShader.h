#pragma once
#include "../../../Shaders/ShaderProgram.h"
#include "glm/gtc/matrix_transform.hpp"

class CEntityShader : public CShaderProgram
{
public:
	virtual void Init() = 0;
	void GetAllUniformLocations();
	void BindAttributes() override;
	void LoadTransformMatrix(const glm::mat4& matrix) const;
	void LoadProjectionMatrix(const glm::mat4& matrix) const;
	void LoadViewMatrix(const glm::mat4& matrix) const;
	void LoadUseBonesTransformation(const float& is) const;
	void LoadBoneTransform(const glm::mat4& transform, uint id) const;
	void LoadUseInstancedRendering(const float& use) const;

protected:
	int location_TransformationMatrix;
	int location_ProjectionMatrix;
	int location_ViewMatrix;

	int location_IsInstancedRender;

	//Animations
	int location_UseBoneTransform;
	int location_Bones[MAX_BONES];
};
