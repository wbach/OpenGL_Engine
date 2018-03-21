#include "PlantsShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	PlantsShader::PlantsShader(IGraphicsApiPtr graphicsApi)
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

	void PlantsShader::GetAllUniformLocations()
	{
		//GetLocation(playerPosition);
	}
	void PlantsShader::BindAttributes()
	{
	}

#define ConnectTerrainTexture(X) LoadValue(uniformLocations.at(UniformLocation::X), Terrain::TexturesTypes::X);

	void PlantsShader::ConnectTextureUnits() const
	{
		//ConnectTerrainTexture(blendMap);
	}
}
