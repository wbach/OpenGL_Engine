#pragma once
#include "../../../Shaders/ShaderProgram.h"

class FontShader : public CShaderProgram
{
public:
	void Init();
	void GetAllUniformLocations() override;
	void BindAttributes() override;
	void loadColour(const glm::vec3& colour) const;
	void loadTranslation(const glm::vec2& pos) const;
	void loadTransformation(const glm::mat4& pos) const;
private:
	int location_translation;
	int location_transformationMatrix;
};
