#include "FontShader.h"
#include "GLM/GLMUtils.h"

void FontShader::Init() {
	CreateProgram();
	AddShader("Font/fontVertex.vert", GL_VERTEX_SHADER);
	AddShader("Font/fontFragment.frag", GL_FRAGMENT_SHADER);
	FinalizeShader();

	Start();
	mat4 scaleMat = Utils::CreateTransformationMatrix(vec2(0), vec2(0.0005, 0.001));
	loadTransformation(scaleMat);
	Stop();
}

void FontShader::GetAllUniformLocations()
{
	location_translation = GetUniformLocation("translation");
	location_transformationMatrix = GetUniformLocation("transformationMatrix");
}

void FontShader::BindAttributes()
{

}

void FontShader::loadColour(const vec3 & colour) const
{
}

void FontShader::loadTranslation(const vec2 & pos) const
{
	LoadValue(location_translation, pos);
}

void FontShader::loadTransformation(const mat4 & pos) const
{
	LoadValue(location_transformationMatrix, pos);
}
