#include "ParticlesShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	ParticlesShader::ParticlesShader(IGraphicsApiPtr graphicsApi)
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

	void ParticlesShader::GetAllUniformLocations()
	{
		//GetLocation(playerPosition);
	}
	void ParticlesShader::BindAttributes()
	{
	}

#define ConnectTerrainTexture(X) LoadValue(uniformLocations.at(UniformLocation::X), Terrain::TexturesTypes::X);

	void ParticlesShader::ConnectTextureUnits() const
	{
		//ConnectTerrainTexture(blendMap);
	}
}
