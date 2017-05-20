#include "SkyBoxShader.h"
#include "glm/gtx/transform.hpp"

void CSkyBoxShader::Init()
{
    CreateProgram();
    AddShader("../Shaders/SkyBox/SkyboxVertexShader.vert", GL_VERTEX_SHADER);
    AddShader("../Shaders/SkyBox/SkyboxFragmentShader.frag", GL_FRAGMENT_SHADER);
    FinalizeShader();

    m_Rotation = 0.f;
    m_RotationSpeed = 0.1f;
}

void CSkyBoxShader::LoadProjectionMatrix(const glm::mat4& matrix) const
{
	LoadValue(location_ProjectionMatrix, matrix);
}

void CSkyBoxShader::LoadViewMatrix(glm::mat4 matrix, const float& deltaTime, const float& distance_view)
{
	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	m_Rotation += m_RotationSpeed * deltaTime;
	matrix *= glm::scale(glm::vec3(distance_view *0.5f));
	matrix *= glm::rotate((float) m_Rotation, .0f, 1.f, .0f);
	LoadValue(location_ViewMatrix, matrix);
}

void CSkyBoxShader::GetAllUniformLocations()
{
	location_ProjectionMatrix = GetUniformLocation("ProjectionMatrix");
	location_ViewMatrix = GetUniformLocation("ViewMatrix");
	location_DayCubeMap = GetUniformLocation("DayCubeMap");
	location_NightCubeMap = GetUniformLocation("NightCubeMap");
	location_FogColour = GetUniformLocation("FogColour");
	location_BlendFactor = GetUniformLocation("BlendFactor");
}

void CSkyBoxShader::LoadFogColour(const float& r, const float& g, const float& b) const
{
	LoadValue(location_FogColour, glm::vec3(r, g, b));
}

void CSkyBoxShader::LoadBlendFactor(const float& factor)const
{
	LoadValue(location_BlendFactor, factor);
}

void CSkyBoxShader::ConnectTextureUnits() const
{
	LoadValue(location_DayCubeMap, 0);
	LoadValue(location_NightCubeMap, 1);
}

void CSkyBoxShader::BindAttributes() const
{
	BindAttribute(0, "position");
}
