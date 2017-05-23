#pragma once
#include "../ShaderProgram.h"
#include <vector>
#include "../../Lights/Light.h"
static const int MAX_LIGHTS = 10;

class CLightPassShader : public CShaderProgram
{
public:
	void GetAllUniformLocations() override;
	void BindAttributes() override;

	void Init();

	void LoadLightNumber(const int& i) const;
	void LoadLight(const CLight& light, const int& i) const;
	void LoadCameraPosition(const glm::vec3& camera_position) const;

	void LoadSkyColour(const glm::vec3& color) const;
	void LoadViewDistance(const float& distance) const;

	void LoadScreenSize(const wb::vec2i& screen_size);
	void ConnectTextureUnits() const override;

private:
	int m_NumberOfLights;
	int location_ScreenSize;

	int location_TransformationMatrix;
	int location_ProjectionMatrix;
	int location_ViewMatrix;

	int location_PositionMap;
	int location_ColorMap;
	int location_NormalMap;

	int location_DepthTexture;

	int location_SpecularMap;
	int location_CameraPosition;

	int location_SkyColour;
	int location_ViewDistance;

	int location_NumberOfLights;
	std::vector<int> location_LightType;
	std::vector<int> location_LightPosition;
	std::vector<int> location_LightColour;
	std::vector<int> location_LightAttenuation;
	std::vector<int> location_LightCutOff;
};
