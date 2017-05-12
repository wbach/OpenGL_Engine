#include "LightPassShader.h"

void CLightPassShader::Init()
{
	CreateProgram();
	AddShader("../Shaders/Light/LightPassShader.vert", GL_VERTEX_SHADER);
	AddShader("../Shaders/Light/LightPassShader.frag", GL_FRAGMENT_SHADER);
	FinalizeShader();
}
void CLightPassShader::GetAllUniformLocations()
{
	location_ScreenSize = GetUniformLocation("ScreenSize");

	location_CameraPosition = GetUniformLocation("CameraPosition");

	// Textures
	location_PositionMap = GetUniformLocation("PositionMap");
	location_ColorMap = GetUniformLocation("ColorMap");
	location_NormalMap = GetUniformLocation("NormalMap");
	location_SpecularMap = GetUniformLocation("SpecularMap");
	location_DepthTexture = GetUniformLocation("DepthTexture");

	location_SkyColour = GetUniformLocation("SkyColour");

	location_ViewDistance = GetUniformLocation("ViewDistance");

	// Lights 
	location_NumberOfLights = GetUniformLocation("NumberOfLights");

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		location_LightType.push_back(GetUniformLocation("lights[" + std::to_string(i) + "].m_Type"));
		location_LightPosition.push_back(GetUniformLocation("lights[" + std::to_string(i) + "].m_Position"));
		location_LightColour.push_back(GetUniformLocation("lights[" + std::to_string(i) + "].m_Colour"));
		location_LightAttenuation.push_back(GetUniformLocation("lights[" + std::to_string(i) + "].m_Attenuation"));
		location_LightCutOff.push_back(GetUniformLocation("lights[" + std::to_string(i) + "].m_CutOff"));
	}
}

void CLightPassShader::BindAttributes()
{
	BindAttribute(0, "Position");
}
void CLightPassShader::LoadLightNumber(const int& i) const
{
	LoadValue(location_NumberOfLights, i);
}
void CLightPassShader::LoadLight(const CLight& light, const int& i) const
{
	LoadValue(location_LightType[i], light.GetType());
	LoadValue(location_LightPosition[i], light.GetPosition());
	LoadValue(location_LightColour[i], light.GetColour());
	LoadValue(location_LightAttenuation[i], light.GetAttenuation());
	LoadValue(location_LightCutOff[i], light.GetCutoff());
}

void CLightPassShader::LoadCameraPosition(const glm::vec3& camera_position) const
{
	LoadValue(location_CameraPosition, camera_position);
}

void CLightPassShader::LoadScreenSize(const wb::vec2i& screen_size)
{
	LoadValue(location_ScreenSize, glm::vec2((float)screen_size.x , (float) screen_size.y));
}

void CLightPassShader::ConnectTextureUnits() const
{
	LoadValue(location_PositionMap, 0);
	LoadValue(location_ColorMap, 1);
	LoadValue(location_NormalMap, 2);
	LoadValue(location_SpecularMap, 3);
	LoadValue(location_DepthTexture, 4);

}
void CLightPassShader::LoadSkyColour(const glm::vec3& color) const
{
	LoadValue(location_SkyColour, color);
}
void CLightPassShader::LoadViewDistance(const float& distance) const
{
	LoadValue(location_ViewDistance, distance);
}