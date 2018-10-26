#include "LoadingShader.h"

namespace GameEngine
{
LoadingShader::LoadingShader(IGraphicsApiPtr ptr)
    : ShaderProgram(ptr)
{
}
void LoadingShader::Init()
{
    SetFiles({
        {"Game/loadingShader.vert", ShaderType::VERTEX_SHADER},
        {"Game/loadingShader.frag", ShaderType::FRAGMENT_SHADER},
    });

    ShaderProgram::Init();
}
void LoadingShader::GetAllUniformLocations()
{
    location_transformationMatrix = GetUniformLocation("transformationMatrix");
    location_modelTexture         = GetUniformLocation("modelTexture");
}
void LoadingShader::LoadIsTextured(const float& is_tex) const
{
    LoadValue(location_isTextured, is_tex);
}
void LoadingShader::ConnectTextureUnits() const
{
    LoadValue(location_modelTexture, 0);
}
void LoadingShader::BindAttributes()
{
    BindAttribute(0, "position");
    BindAttribute(1, "textureCoords");
}
void LoadingShader::LoadTransformMatrix(const mat4& matrix) const
{
    LoadValue(location_transformationMatrix, matrix);
}
void LoadingShader::LoadProjectionMatrix(const mat4& matrix) const
{
    LoadValue(location_projectionMatrix, matrix);
}
void LoadingShader::LoadViewMatrix(const mat4& matrix) const
{
    LoadValue(location_viewMatrix, matrix);
}
LoadingShader::~LoadingShader()
{
}
}  // namespace GameEngine
