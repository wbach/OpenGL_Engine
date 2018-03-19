#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"
#include <unordered_map>

namespace GameEngine
{
	class TerrainShader : public CShaderProgram
	{		
	public:
		enum UniformLocation
		{
			playerPosition,
			lightDirection,
			modelViewProjectionMatrix,
			displacementTexture,
			modelViewMatrix,
			projectionMatrix,
			heightFactor,
			toShadowMapSpace,
			shadowVariables,
			shadowMap,
			blendMap,
			backgorundTexture,
			backgorundTextureNormal,
			redTexture,
			redTextureNormal,
			greenTexture,
			greenTextureNormal,
			blueTexture,
			blueTextureNormal,
			rockTexture,
			rockNormalTexture,
			snowTexture,
			snowNormalTexture,
			displacementMap
		};

		TerrainShader(IGraphicsApiPtr graphicsApi);
		void GetAllUniformLocations();
		void BindAttributes() override;
		void ConnectTextureUnits() const override;

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

	protected:
		std::unordered_map<UniformLocation, uint32> uniformLocations;
	};
} // GameEngine


/*
TransformMatrix,
ProjectionMatrix,
ViewMatrix,
Viewport,
ScreenSize,
LodFactor,
HeightFactor,
ViewDistance,
BlendMap,
BackgroundTexture,
rTexture,
bTexture,
gTexture,
RockTexture,
BackgroundNormalTexture,
rNormalTexture,
bNormalTexture,
gNormalTexture,
RockNormalTexture,
DisplacementMap,
distanceDev,
minTessLevelOuter,
shadowMap,
*/