#include "TerrainShader.h"

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
		uniformLocations[UniformLocation::modelViewProjectionMatrix] = GetUniformLocation("modelViewProjectionMatrix");
		uniformLocations[UniformLocation::modelViewMatrix] = GetUniformLocation("modelViewMatrix");
		uniformLocations[UniformLocation::projectionMatrix]  = GetUniformLocation("projectionMatrix");
		uniformLocations[UniformLocation::heightFactor] = GetUniformLocation("heightFactor");

		uniformLocations[UniformLocation::displacementTexture] = GetUniformLocation("displacementTexture");

		uniformLocations[UniformLocation::BlendMap] = GetUniformLocation("BlendMap");
		uniformLocations[UniformLocation::BackgroundTexture] = GetUniformLocation("BackgroundTexture");
		uniformLocations[UniformLocation::rTexture] = GetUniformLocation("rTexture");
		uniformLocations[UniformLocation::bTexture] = GetUniformLocation("bTexture");
		uniformLocations[UniformLocation::gTexture] = GetUniformLocation("gTexture");
		uniformLocations[UniformLocation::RockTexture] = GetUniformLocation("RockTexture");
		
		uniformLocations[UniformLocation::BackgroundNormalTexture] = GetUniformLocation("BackgroundTextureNormal");
		uniformLocations[UniformLocation::rNormalTexture] = GetUniformLocation("rTextureNormal");
		uniformLocations[UniformLocation::bNormalTexture] = GetUniformLocation("bTextureNormal");
		uniformLocations[UniformLocation::gNormalTexture] = GetUniformLocation("gTextureNormal");
		uniformLocations[UniformLocation::RockNormalTexture] = GetUniformLocation("RockTextureNormal");

		uniformLocations[UniformLocation::shadowMap] = GetUniformLocation("shadowMap");
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