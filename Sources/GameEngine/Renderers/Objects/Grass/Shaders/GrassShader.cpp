#include "GrassShader.h"
#include "GrassShaderUniforms.h"

namespace GameEngine
{
GrassShader::GrassShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Grass)
{
}

void GrassShader::ConnectTextureUnits() const
{
    Load(GrassShaderUniforms::ColorMap, 0);
    Load(GrassShaderUniforms::ShadowMap, 1);
}

void GrassShader::GetAllUniformLocations()
{
    uniforms_.resize(GrassShaderUniforms::SIZE);

    uniforms_[GrassShaderUniforms::ViewMatrix]       = GetUniformLocation("ViewMatrix");
    uniforms_[GrassShaderUniforms::ProjectionMatrix] = GetUniformLocation("ProjectionMatrix");
    uniforms_[GrassShaderUniforms::ColorMap]         = GetUniformLocation("Texture0");
    uniforms_[GrassShaderUniforms::GlobalTime]       = GetUniformLocation("GlobalTime");
    uniforms_[GrassShaderUniforms::ShadowVariables]  = GetUniformLocation("ShadowVariables");
    uniforms_[GrassShaderUniforms::ShadowMap]        = GetUniformLocation("ShadowMap");
    uniforms_[GrassShaderUniforms::ToShadowMapSpace] = GetUniformLocation("ToShadowMapSpace");
    uniforms_[GrassShaderUniforms::ViewDistance]     = GetUniformLocation("ViewDistance");
}

void GrassShader::BindAttributes()
{
    BindAttribute(0, "Position");
}
}  // namespace GameEngine
