#include "EntityShader.h"
#include "EntityShaderUniforms.h"

namespace GameEngine
{
EntityShader::EntityShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Entity)
{
}

void EntityShader::GetAllUniformLocations()
{
    uniforms_.resize(EntityShaderUniforms::SIZE);

    uniforms_[EntityShaderUniforms::ColorMap] = GetUniformLocation("DiffuseTexture");
}
void EntityShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "Normal");
    BindAttribute(3, "Tangent");
    BindAttribute(4, "Weights");
    BindAttribute(5, "BoneIds");
}
void EntityShader::ConnectTextureUnits() const
{
    Load(EntityShaderUniforms::ColorMap, 0);
}
}  // namespace GameEngine
