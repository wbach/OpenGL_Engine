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

    //uniforms_[EntityShaderUniforms::ViewMatrix]           = GetUniformLocation("ViewMatrix");
    //uniforms_[EntityShaderUniforms::ProjectionMatrix]     = GetUniformLocation("ProjectionMatrix");
    //uniforms_[EntityShaderUniforms::TransformationMatrix] = GetUniformLocation("TransformationMatrix");
    //uniforms_[EntityShaderUniforms::TextureOffset]        = GetUniformLocation("TextureOffset");
    //uniforms_[EntityShaderUniforms::NumberOfRows]         = GetUniformLocation("NumberOfRows");
    //uniforms_[EntityShaderUniforms::UseBoneTransform]     = GetUniformLocation("UseBoneTransform");
    //uniforms_[EntityShaderUniforms::BonesTransforms]      = GetUniformLocation("BonesTransforms");
    uniforms_[EntityShaderUniforms::ColorMap]             = GetUniformLocation("gColorMap");
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
