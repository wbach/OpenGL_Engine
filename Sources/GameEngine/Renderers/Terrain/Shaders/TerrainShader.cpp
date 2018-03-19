#include "TerrainShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	TerrainShader::TerrainShader(IGraphicsApiPtr graphicsApi)
		: CShaderProgram(graphicsApi)
	{
		SetFiles
		({
			{ "Terrain/TerrainShader.vert", ShaderType::VERTEX_SHADER },
			{ "Terrain/TerrainShader.frag", ShaderType::FRAGMENT_SHADER },
			{ "Terrain/TerrainShader.cs",	ShaderType::TESS_CONTROL_SHADER },
			{ "Terrain/TerrainShader.es",	ShaderType::TESS_EVALUATION_SHADER }
		});
	}

	void TerrainShader::GetAllUniformLocations()
	{
		GetLocation(playerPosition);
		GetLocation(lightDirection);
		GetLocation(modelViewProjectionMatrix);
		GetLocation(displacementTexture);
		GetLocation(modelViewMatrix);
		GetLocation(projectionMatrix);
		GetLocation(heightFactor);
		GetLocation(toShadowMapSpace);
		GetLocation(shadowVariables);
		GetLocation(shadowMap);
		GetLocation(blendMap);
		GetLocation(backgorundTexture);
		GetLocation(backgorundTextureNormal);
		GetLocation(redTexture);
		GetLocation(redTextureNormal);
		GetLocation(greenTexture);
		GetLocation(greenTextureNormal);
		GetLocation(blueTexture);
		GetLocation(blueTextureNormal);
		GetLocation(rockTexture);
		GetLocation(rockNormalTexture);
		GetLocation(snowTexture);
		GetLocation(snowNormalTexture);
		GetLocation(displacementMap);
	}
	void TerrainShader::BindAttributes()
	{
	}

#define ConnectTerrainTexture(X) LoadValue(uniformLocations.at(UniformLocation::X), Terrain::TexturesTypes::X);

	void TerrainShader::ConnectTextureUnits() const
	{
		if (uniformLocations.count(UniformLocation::shadowMap) > 0)
			LoadValue(uniformLocations.at(UniformLocation::shadowMap), 0);

		ConnectTerrainTexture(blendMap);
		ConnectTerrainTexture(backgorundTexture);
		ConnectTerrainTexture(backgorundTextureNormal);
		ConnectTerrainTexture(redTexture);
		ConnectTerrainTexture(redTextureNormal);
		ConnectTerrainTexture(greenTexture);
		ConnectTerrainTexture(greenTextureNormal);
		ConnectTerrainTexture(blueTexture);
		ConnectTerrainTexture(blueTextureNormal);
		ConnectTerrainTexture(rockTexture);
		ConnectTerrainTexture(rockNormalTexture);
		ConnectTerrainTexture(snowTexture);
		ConnectTerrainTexture(snowNormalTexture);
		ConnectTerrainTexture(displacementMap);
	}
}