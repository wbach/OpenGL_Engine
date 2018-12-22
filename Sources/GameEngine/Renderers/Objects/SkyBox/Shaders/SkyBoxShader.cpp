#include "SkyBoxShader.h"
#include "glm/gtx/transform.hpp"
#include "SkyBoxShaderUniforms.h"

namespace GameEngine
{
SkyBoxShader::SkyBoxShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::SkyBox))
{

}

void SkyBoxShader::GetAllUniformLocations()
{
    uniforms_.resize(SkyBoxShaderUniforms::SIZE);

    uniforms_[SkyBoxShaderUniforms::ViewMatrix]       = GetUniformLocation("ProjectionMatrix");
    uniforms_[SkyBoxShaderUniforms::ProjectionMatrix] = GetUniformLocation("ViewMatrix");
    uniforms_[SkyBoxShaderUniforms::DayCubeMap]       = GetUniformLocation("DayCubeMap");
    uniforms_[SkyBoxShaderUniforms::NightCubeMap]     = GetUniformLocation("NightCubeMap");
    uniforms_[SkyBoxShaderUniforms::FogColour]        = GetUniformLocation("FogColour");
    uniforms_[SkyBoxShaderUniforms::BlendFactor]      = GetUniformLocation("BlendFactor");
}

void SkyBoxShader::ConnectTextureUnits() const
{
    Load(SkyBoxShaderUniforms::DayCubeMap, 0);
    Load(SkyBoxShaderUniforms::NightCubeMap, 1);
}

void SkyBoxShader::BindAttributes() const
{
    BindAttribute(0, "position");
}
}  // namespace GameEngine
