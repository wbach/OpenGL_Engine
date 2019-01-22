#include "TreeShader.h"
#include "TreeShaderUniforms.h"

#define GetLocation(X) uniforms_[TreeShaderUniforms::X] = GetUniformLocation(#X)

namespace GameEngine
{
TreeShader::TreeShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Tree)
{
}

void TreeShader::GetAllUniformLocations()
{
    uniforms_.resize(TreeShaderUniforms::SIZE);

    GetLocation(ViewMatrix);
    GetLocation(ProjectionMatrix);
    GetLocation(DiffuseTexture);
    GetLocation(NormalizationMatrix);
    GetLocation(UseShading);
    GetLocation(PositionMap);
    GetLocation(PositionMapSize);
    GetLocation(CameraPosition);

    uniforms_[TreeShaderUniforms::ModelMaterial_Ambient]     = GetUniformLocation("ModelMaterial.ambient");
    uniforms_[TreeShaderUniforms::ModelMaterial_Diffuse]     = GetUniformLocation("ModelMaterial.diffuse");
    uniforms_[TreeShaderUniforms::ModelMaterial_Specular]    = GetUniformLocation("ModelMaterial.specular");
    uniforms_[TreeShaderUniforms::ModelMaterial_ShineDumper] = GetUniformLocation("ModelMaterial.shineDamper");
}
void TreeShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "Normal");
}

void TreeShader::ConnectTextureUnits() const
{
    Load(TreeShaderUniforms::PositionMap, 0);
    Load(TreeShaderUniforms::DiffuseTexture, 1);
}
}  // namespace GameEngine
