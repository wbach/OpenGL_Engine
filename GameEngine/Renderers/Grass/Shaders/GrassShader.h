#pragma once
#include "../../../Shaders/ShaderProgram.h"

class CGrassShader : public CShaderProgram
{
public:
	void Init();
	void LoadTransformMatrix(const glm::mat4&) const;
	void LoadProjectionMatrix(const glm::mat4&) const;
	void LoadViewMatrix(const glm::mat4&) const;

	void LoadGlobalTime(const float& time) const;
	void LoadViewDistance(const float& distance) const;

	void LoadToShadowSpaceMatrix(const glm::mat4& matrix) const;
	void LoadShadowValues(const float& is, const float& distance, const float& shadow_map_size) const;

	void ConnectTextureUnits() const override;

	void GetAllUniformLocations() override;
	void BindAttributes() override;
private:
	int location_TransformationMatrix;
	int location_ProjectionMatrix;
	int location_ViewMatrix;

	int location_Texture;
	int location_GlobalTime;

	int location_ViewDistance;

	//Shadows Variables
	int location_ShadowVariables;
	int location_ShadowMap;
	int location_ToShadowMapSpace;
};
