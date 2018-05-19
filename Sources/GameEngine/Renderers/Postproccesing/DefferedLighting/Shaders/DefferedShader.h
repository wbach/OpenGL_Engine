#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"
#include <unordered_map>

static const uint32 MAX_LIGHTS = 10;

namespace GameEngine
{
class Light;

class DefferedShader : public CShaderProgram
{
private:
	struct LightParamsLocations
	{
		int type;
		int position;
		int colour;
		int attenuation;
		int direction;
		int cutOff;
	};
public:
	enum UniformLocation
	{
		PositionMap,
		ColorMap,
		NormalMap,
		SpecularMap,
		DepthTexture,
		CameraPosition,
		ScreenSize,
		NumberOfLights,
		SkyColour,
		ViewDistance,
	};
	DefferedShader(IGraphicsApiPtr graphicsApi);

	template <class T>
	void Load(UniformLocation location, T value) const
	{
		if (uniformLocations.count(location) == 0)
		{
			Log("CTerrainShader : Try load to shader not set variable : " + std::to_string(location));
			return;
		}
		LoadValue(uniformLocations.at(location), value);
	}
	void LoadLight(uint32 index, const Light& light);

private:
	virtual void BindAttributes() override;
	virtual void GetAllUniformLocations() override;
	virtual void ConnectTextureUnits() const override;

protected:
	std::unordered_map<UniformLocation, uint32> uniformLocations;
	LightParamsLocations lightParamsLocations_[MAX_LIGHTS];
};
} // GameEngine
