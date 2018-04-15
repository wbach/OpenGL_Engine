#include "LoadingShader.h"

CLoadingShader::CLoadingShader(GameEngine::IGraphicsApiPtr ptr)
	: CShaderProgram(ptr)
{
}

void CLoadingShader::Init()
{
	SetFiles
	({
		{"Game/loadingShader.vert", GameEngine::ShaderType::VERTEX_SHADER},
		{"Game/loadingShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER},
	});

	CShaderProgram::Init();
}
void CLoadingShader::GetAllUniformLocations() 
{
	location_transformationMatrix = GetUniformLocation("transformationMatrix");
	location_modelTexture = GetUniformLocation("modelTexture");
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
CLoadingShader::~CLoadingShader() 
{
}
