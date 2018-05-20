#include "DefferedShader.h"
#include "GameEngine/Lights/Light.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	DefferedShader::DefferedShader(IGraphicsApiPtr graphicsApi)
		: CShaderProgram(graphicsApi)
	{
		SetFiles
		({
			{ "Light/LightPassShader.vert", GameEngine::ShaderType::VERTEX_SHADER },
			{ "Light/LightPassShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER },
		});

		CShaderProgram::Init();
	}

	void DefferedShader::GetAllUniformLocations()
	{
		GetLocation(PositionMap);
		GetLocation(ColorMap);
		GetLocation(NormalMap);
		GetLocation(SpecularMap);
		GetLocation(DepthTexture);
		GetLocation(CameraPosition);
		GetLocation(ScreenSize);
		GetLocation(NumberOfLights);
		GetLocation(SkyColour);
		GetLocation(ViewDistance);

		for (uint32 i = 0; i < MAX_LIGHTS; ++i)
		{
			lightParamsLocations_[i].type = GetUniformLocation("Lights[" + std::to_string(i) + "].type_");
			lightParamsLocations_[i].position = GetUniformLocation("Lights[" + std::to_string(i) + "].position_");
			lightParamsLocations_[i].colour = GetUniformLocation("Lights[" + std::to_string(i) + "].colour_");
			lightParamsLocations_[i].attenuation = GetUniformLocation("Lights[" + std::to_string(i) + "].attenuation_");
			lightParamsLocations_[i].cutOff = GetUniformLocation("Lights[" + std::to_string(i) + "].cutOff_");
		}
	}
	void DefferedShader::LoadLight(uint32 index, const Light & light)
	{
		const auto& l = lightParamsLocations_[index];
		LoadValue(l.type, static_cast<int>(light.GetType()));
		LoadValue(l.attenuation, light.GetAttenuation());
		LoadValue(l.colour, light.GetColour());
		LoadValue(l.cutOff, light.GetCutoff());
		LoadValue(l.position, light.GetPosition());
		LoadValue(l.direction, light.GetDirection());
	}
	void DefferedShader::BindAttributes()
	{
		BindAttribute(0, "Position");
		BindAttribute(1, "TexCoord");
	}
	void DefferedShader::ConnectTextureUnits() const
	{
		LoadValue(uniformLocations.at(PositionMap), 0);
		LoadValue(uniformLocations.at(ColorMap), 1);
		LoadValue(uniformLocations.at(NormalMap), 2);
		LoadValue(uniformLocations.at(SpecularMap), 3);
		LoadValue(uniformLocations.at(DepthTexture), 4);
	}
}// GameEngine
