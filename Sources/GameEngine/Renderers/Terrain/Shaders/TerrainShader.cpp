#include "TerrainShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	TerrainShader::TerrainShader()
	{
		SetFiles
		({
			{ "Terrain/TerrainShader.vert", GL_VERTEX_SHADER },
			{ "Terrain/TerrainShader.frag", GL_FRAGMENT_SHADER },
			{ "Terrain/TerrainShader.cs", GL_TESS_CONTROL_SHADER },
			{ "Terrain/TerrainShader.es", GL_TESS_EVALUATION_SHADER }
		});
	}

	void TerrainShader::GetAllUniformLocations()
	{
		GetLocation(playerPosition);
		GetLocation(modelViewProjectionMatrix);
		GetLocation(modelViewMatrix);
		GetLocation(projectionMatrix);
		GetLocation(heightFactor);
		GetLocation(displacementTexture);
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