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

    uniforms_[EntityShaderUniforms::ClipPlane]             = GetUniformLocation("ClipPlane");
    uniforms_[EntityShaderUniforms::IsUseFakeLighting]     = GetUniformLocation("IsUseFakeLighting");
    uniforms_[EntityShaderUniforms::IsUseNormalMap]        = GetUniformLocation("IsUseNormalMap");
    uniforms_[EntityShaderUniforms::UseTexture]            = GetUniformLocation("UseTexture");
    uniforms_[EntityShaderUniforms::NormalMap]             = GetUniformLocation("NormalMap");
    uniforms_[EntityShaderUniforms::ShadowVariables]       = GetUniformLocation("ShadowVariables");
    uniforms_[EntityShaderUniforms::ShadowMap]             = GetUniformLocation("ShadowMap");
    uniforms_[EntityShaderUniforms::ToShadowMapSpace]      = GetUniformLocation("ToShadowMapSpace");
    uniforms_[EntityShaderUniforms::ModelMaterialAmbient]  = GetUniformLocation("ModelMaterial.m_Ambient");
    uniforms_[EntityShaderUniforms::ModelMaterialDiffuse]  = GetUniformLocation("ModelMaterial.m_Diffuse");
    uniforms_[EntityShaderUniforms::ModelMaterialSpecular] = GetUniformLocation("ModelMaterial.m_Specular");
    uniforms_[EntityShaderUniforms::ViewDistance]          = GetUniformLocation("ViewDistance");
}

void DefferedEntityShader::ConnectTextureUnits() const
{
    EntityShader::ConnectTextureUnits();

    Load(EntityShaderUniforms::NormalMap, 2);
    Load(EntityShaderUniforms::ShadowMap, 3);
}
void DefferedEntityShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "Normal");
    BindAttribute(3, "Tangent");
    BindAttribute(4, "Weights");
    BindAttribute(5, "BoneIds");
    //BindAttribute(4, "TransformationMatrixes");
}
}  // namespace GameEngine
