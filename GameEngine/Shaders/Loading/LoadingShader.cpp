#include "LoadingShader.h"

void CLoadingShader::Init()
{
	CreateProgram();
    AddShader("Game/loadingShader.vert", GL_VERTEX_SHADER);
    AddShader("Game/loadingShader.frag", GL_FRAGMENT_SHADER);
	FinalizeShader();
}
void CLoadingShader::GetAllUniformLocations() 
{
	location_isTextured = GetUniformLocation("isTextured");
	location_transformationMatrix = GetUniformLocation("transformationMatrix");
	location_projectionMatrix = GetUniformLocation("projectionMatrix");
	location_viewMatrix = GetUniformLocation("viewMatrix");
	location_modelTexture = GetUniformLocation("modelTexture");
	location_alpha = GetUniformLocation("alphaBlend");
}
void CLoadingShader::LoadIsTextured(const float& is_tex) const 
{
	LoadValue(location_isTextured, is_tex);

}
void CLoadingShader::ConnectTextureUnits() const
{
	LoadValue(location_modelTexture, 0);
}
void CLoadingShader::BindAttributes()
{
	BindAttribute(0, "position");
	BindAttribute(1, "textureCoords");
}
void CLoadingShader::LoadTransformMatrix(const mat4& matrix) const 
{
	LoadValue(location_transformationMatrix, matrix);
}
void CLoadingShader::LoadProjectionMatrix(const mat4& matrix) const
{
	LoadValue(location_projectionMatrix, matrix);
}
void CLoadingShader::LoadViewMatrix(const mat4& matrix) const
{
	LoadValue(location_viewMatrix, matrix);
}
void CLoadingShader::LoadAlphaValue(const float& alpha) const
{
	LoadValue(location_alpha, alpha);
}
CLoadingShader::~CLoadingShader() 
{
}
