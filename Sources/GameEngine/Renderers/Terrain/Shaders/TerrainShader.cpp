#include "TerrainShader.h"

namespace GameEngine
{
	TerrainShader::TerrainShader()
	{
	}

	void TerrainShader::Init()
	{
		SetFiles
		({
			{ "Terrain/TerrainShader.vert", GL_VERTEX_SHADER },
			{ "Terrain/TerrainShader.frag", GL_FRAGMENT_SHADER },
			{ "Terrain/TerrainShader.cs", GL_TESS_CONTROL_SHADER },
			{ "Terrain/TerrainShader.es", GL_TESS_EVALUATION_SHADER }
			});

		CShaderProgram::Init();
	}
	void TerrainShader::GetAllUniformLocations()
	{
		uniformLocations[UniformLocation::transformMatrix] = GetUniformLocation("transformMatrix");
		uniformLocations[UniformLocation::mvp_matrix] = GetUniformLocation("mvp_matrix");
		uniformLocations[UniformLocation::mv_matrix]  = GetUniformLocation("mv_matrix");
		uniformLocations[UniformLocation::proj_matrix] = GetUniformLocation("proj_matrix");
		uniformLocations[UniformLocation::dmap_depth] = GetUniformLocation("dmap_depth");

		uniformLocations[UniformLocation::tex_displacement] = GetUniformLocation("tex_displacement");

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
		LoadValue(uniformLocations.at(UniformLocation::tex_displacement), 11);
		LoadValue(uniformLocations.at(UniformLocation::shadowMap), 12);
	}
}