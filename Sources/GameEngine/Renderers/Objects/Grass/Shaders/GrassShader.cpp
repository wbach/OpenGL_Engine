#include "GrassShader.h"

CGrassShader::CGrassShader(GameEngine::IGraphicsApiPtr graphicsApi)
	: CShaderProgram(graphicsApi)
{
}

void CGrassShader::Init()
{
	SetFiles
	({
		{ "Grass/GrassShader.vert", GameEngine::ShaderType::VERTEX_SHADER },
		{ "Grass/GrassShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER },
		{ "Grass/GrassShader.geom", GameEngine::ShaderType::GEOMETRY_SHADER }
	});

	CShaderProgram::Init();
}

void CGrassShader::LoadTransformMatrix(const mat4 & m) const
{
	LoadValue(location_TransformationMatrix, m);
}

void CGrassShader::LoadProjectionMatrix(const mat4 &m) const
{
	LoadValue(location_ProjectionMatrix, m);
}

void CGrassShader::LoadViewMatrix(const mat4 &m) const
{
	LoadValue(location_ViewMatrix, m);
}

void CGrassShader::LoadGlobalTime(const float & time) const
{
	LoadValue(location_GlobalTime, time);
}

void CGrassShader::LoadToShadowSpaceMatrix(const mat4 & matrix) const
{
	LoadValue(location_ToShadowMapSpace, matrix);
}

void CGrassShader::LoadShadowValues(const float & is, const float & distance, const float & shadow_map_size) const
{
	LoadValue(location_ShadowVariables, vec3(is, distance - 5, shadow_map_size));
}
void CGrassShader::LoadViewDistance(const float& distance) const
{
	LoadValue(location_ViewDistance, distance);
}
void CGrassShader::ConnectTextureUnits() const
{
	LoadValue(location_Texture, 0);
	LoadValue(location_ShadowMap, 1);
}

void CGrassShader::GetAllUniformLocations()
{
	location_ProjectionMatrix = GetUniformLocation("ProjectionMatrix");
	location_ViewMatrix = GetUniformLocation("ViewMatrix");

	//Textures
	location_Texture = GetUniformLocation("Texture0");

	location_GlobalTime = GetUniformLocation("GlobalTime");

	//Shadows
	location_ShadowVariables = GetUniformLocation("ShadowVariables");
	location_ShadowMap = GetUniformLocation("ShadowMap");
	location_ToShadowMapSpace = GetUniformLocation("ToShadowMapSpace");

	//Skip render
	location_ViewDistance = GetUniformLocation("ViewDistance");
}

void CGrassShader::BindAttributes()
{
	BindAttribute(0, "Position");
}
