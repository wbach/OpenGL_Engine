#include "TerrainShader.h"

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
		GetLocation(BlendMap);
		GetLocation(BackgroundTexture);
		GetLocation(rTexture);
		GetLocation(bTexture);
		GetLocation(gTexture);
		GetLocation(RockTexture);
		GetLocation(BackgroundNormalTexture);
		GetLocation(rNormalTexture);
		GetLocation(bNormalTexture);
		GetLocation(gNormalTexture);
		GetLocation(RockNormalTexture);
		GetLocation(shadowMap);
	}
	void TerrainShader::BindAttributes()
	{
	}
	void TerrainShader::ConnectTextureUnits() const
	{
		LoadValue(uniformLocations.at(UniformLocation::BackgroundTexture), 0);
		LoadValue(uniformLocations.at(UniformLocation::rTexture), 1);
		LoadValue(uniformLocations.at(UniformLocation::gTexture), 2);
		LoadValue(uniformLocations.at(UniformLocation::bTexture), 3);
		LoadValue(uniformLocations.at(UniformLocation::BlendMap), 4);
		LoadValue(uniformLocations.at(UniformLocation::BackgroundNormalTexture), 5);
		LoadValue(uniformLocations.at(UniformLocation::rNormalTexture), 6);
		LoadValue(uniformLocations.at(UniformLocation::gNormalTexture), 7);
		LoadValue(uniformLocations.at(UniformLocation::bNormalTexture), 8);
		LoadValue(uniformLocations.at(UniformLocation::RockTexture), 9);
		LoadValue(uniformLocations.at(UniformLocation::RockNormalTexture), 10);
		LoadValue(uniformLocations.at(UniformLocation::displacementTexture), 11);
		LoadValue(uniformLocations.at(UniformLocation::shadowMap), 12);
	}
}