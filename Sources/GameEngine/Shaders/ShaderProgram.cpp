#include "ShaderProgram.h"

#include <Logger/Log.h>

namespace GameEngine
{
ShaderProgram::ShaderProgram(GraphicsApi::IGraphicsApi& graphicsApi, GraphicsApi::ShaderProgramType shaderType)
    : graphicsApi_(graphicsApi)
    , shaderType_(shaderType)
{
}

ShaderProgram::~ShaderProgram()
{
    Clear();
}
void ShaderProgram::Init()
{
    if (programID_)
    {
        LOG_DEBUG << "Shader program is already initialized.";
        return;
    }

    programID_ = graphicsApi_.CreateShader(shaderType_);
}
void ShaderProgram::Reload()
{
    Clear();
    Init();
}
bool ShaderProgram::IsReady() const
{
    return programID_.has_value();
}

void ShaderProgram::Start() const
{
    if (programID_)
        graphicsApi_.UseShader(*programID_);
}
void ShaderProgram::Stop() const
{
    graphicsApi_.UseShader(0);
}
void ShaderProgram::Clear()
{
    Stop();

    if (programID_)
    {
        graphicsApi_.DeleteObject(*programID_);
        programID_ = std::nullopt;
    }
}
}  // namespace GameEngine
