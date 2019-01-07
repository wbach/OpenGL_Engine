#include "LoadingShader.h"
#include "LoadingShaderUnfiorms.h"

namespace GameEngine
{
LoadingShader::LoadingShader(IGraphicsApiPtr ptr)
    : ShaderProgram(ptr, ptr->GetShaderFiles(Shaders::Loading))
{
}

void LoadingShader::ConnectTextureUnits() const
{
    Load(LoadingShaderUniforms::ModelTexture, 0);
}

void LoadingShader::GetAllUniformLocations()
{
    uniforms_.resize(LoadingShaderUniforms::SIZE - 1);
    uniforms_[LoadingShaderUniforms::TransformMatrix] = GetUniformLocation("transformationMatrix");
    uniforms_[LoadingShaderUniforms::ModelTexture]    = GetUniformLocation("modelTexture");
}

void LoadingShader::BindAttributes()
{
    BindAttribute(0, "position");
    BindAttribute(1, "textureCoords");
}
}  // namespace GameEngine
