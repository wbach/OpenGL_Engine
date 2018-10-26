#include "ParticlesShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
ParticlesShader::ParticlesShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
    SetFiles({
        {"Particles/ParticlesShader.vert", ShaderType::VERTEX_SHADER},
        {"Particles/ParticlesShader.frag", ShaderType::FRAGMENT_SHADER},
    });
}
void ParticlesShader::GetAllUniformLocations()
{
    GetLocation(ProjectionMatrix);
    GetLocation(ModelViewMatrix);
    GetLocation(ParticleTexture);
}
void ParticlesShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "ModelViewMatrix");
    BindAttribute(6, "TextureOffset");
    BindAttribute(7, "NumberOfRows");
}
void ParticlesShader::ConnectTextureUnits() const
{
    LoadValue(uniformLocations.at(UniformLocation::ParticleTexture), 0);
}
}
