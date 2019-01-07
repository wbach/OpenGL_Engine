#include "ShadowShader.h"
#include "ShadowShaderUniforms.h"

#define GetLocation(X) uniformLocations[UniformLocation::X].push_back(GetUniformLocation(#X))

namespace GameEngine
{
ShadowShader::ShadowShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::Shadows))
{
}

void ShadowShader::GetAllUniformLocations()
{
    uniforms_.resize(ShadowShaderUniforms::SIZE);

    uniforms_[ShadowShaderUniforms::TransformationMatrix] = GetUniformLocation("TransformationMatrix");
    uniforms_[ShadowShaderUniforms::ProjectionViewMatrix] = GetUniformLocation("ProjectionViewMatrix");
    uniforms_[ShadowShaderUniforms::UseBoneTransform]     = GetUniformLocation("UseBoneTransform");
    uniforms_[ShadowShaderUniforms::NumberOfRows]         = GetUniformLocation("NumberOfRows");
    uniforms_[ShadowShaderUniforms::TextureOffset]        = GetUniformLocation("TextureOffset");
    uniforms_[ShadowShaderUniforms::ModelTexture]         = GetUniformLocation("ModelTexture");
    uniforms_[ShadowShaderUniforms::BonesTransforms]      = GetUniformLocation("BonesTransforms");
}

void ShadowShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(4, "Weights");
    BindAttribute(5, "BoneIds");
}
void ShadowShader::ConnectTextureUnits() const
{
    Load(ShadowShaderUniforms::ModelTexture, 1);
}

}// namespace GameEngine
