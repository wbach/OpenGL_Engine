#include "FontShader.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
FontShader::FontShader(GameEngine::IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
    , baseScale_(0.0005f, 0.001f)
{
}

void FontShader::Init()
{
    SetFiles({{"Font/fontVertex.vert", GameEngine::ShaderType::VERTEX_SHADER},
              {"Font/fontFragment.frag", GameEngine::ShaderType::FRAGMENT_SHADER}});

    ShaderProgram::Init();

    Start();
    loadTransformation(Utils::CreateTransformationMatrix(vec2(0), baseScale_));
    Stop();
}

void FontShader::SetScale(float scale) const
{
    loadTransformation(Utils::CreateTransformationMatrix(vec2(0), baseScale_ * scale));
}

void FontShader::GetAllUniformLocations()
{
    location_color                = GetUniformLocation("color");
    location_translation          = GetUniformLocation("translation");
    location_transformationMatrix = GetUniformLocation("transformationMatrix");
}

void FontShader::BindAttributes()
{
}

void FontShader::LoadColour(const vec3& colour) const
{
    LoadValue(location_color, colour);
}

void FontShader::LoadTranslation(const vec2& pos) const
{
    LoadValue(location_translation, pos);
}

void FontShader::loadTransformation(const mat4& pos) const
{
    LoadValue(location_transformationMatrix, pos);
}
}  // namespace GameEngine
