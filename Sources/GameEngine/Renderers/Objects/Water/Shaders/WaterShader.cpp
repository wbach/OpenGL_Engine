#include "WaterShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
WaterShader::WaterShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
    SetFiles({{"Terrain/TerrainShader.vert", ShaderType::VERTEX_SHADER},
              {"Terrain/TerrainShader.frag", ShaderType::FRAGMENT_SHADER},
              {"Terrain/TerrainShader.cs", ShaderType::TESS_CONTROL_SHADER},
              {"Terrain/TerrainShader.es", ShaderType::TESS_EVALUATION_SHADER}});
}

void WaterShader::GetAllUniformLocations()
{
    // GetLocation(playerPosition);
}
void WaterShader::BindAttributes()
{
}

#define ConnectTerrainTexture(X) LoadValue(uniformLocations.at(UniformLocation::X), Terrain::TexturesTypes::X);

void WaterShader::ConnectTextureUnits() const
{
    // ConnectTerrainTexture(blendMap);
}
}
