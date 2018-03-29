#pragma once
#include "../../../Shaders/ShaderProgram.h"

class CEntityShader : public CShaderProgram
{
public:
	CEntityShader(GameEngine::IGraphicsApiPtr graphicsApi);
	virtual void Init() = 0;
	void GetAllUniformLocations();
	void BindAttributes() override;
	void LoadTransformMatrix(const mat4& matrix) const;
	void LoadProjectionMatrix(const mat4& matrix) const;
	void LoadViewMatrix(const mat4& matrix) const;
	void LoadUseBonesTransformation(const float& is) const;
	void LoadBoneTransform(const mat4& transform, uint32 id) const;
	void LoadTextureOffset(const vec2& offset) const;
	void LoadNumberOfRows(uint32 rows) const;

protected:
	int location_TransformationMatrix;
	int location_ProjectionMatrix;
	int location_ViewMatrix;

	//Animations
	int location_UseBoneTransform;
	int location_Bones[MAX_BONES];

	int location_TextureOffset;
	int location_NumberOfRows;
};
