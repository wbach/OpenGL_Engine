#include "DefferedEntityShader.h"
#include "EntityShaderUniforms.h"

namespace GameEngine
{
DefferedEntityShader::DefferedEntityShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : EntityShader(graphicsApi)
{
}

void DefferedEntityShader::GetAllUniformLocations()
{
    EntityShader::GetAllUniformLocations();

    uniforms_[EntityShaderUniforms::NormalMap] = GetUniformLocation("NormalMap");
    uniforms_[EntityShaderUniforms::ShadowMap] = GetUniformLocation("ShadowMap");
}

void DefferedEntityShader::ConnectTextureUnits() const
{
    EntityShader::ConnectTextureUnits();

    Load(EntityShaderUniforms::NormalMap, 2);
    Load(EntityShaderUniforms::ShadowMap, 3);
}
}  // namespace GameEngine
