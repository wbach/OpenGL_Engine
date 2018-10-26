#include "SimpleEnityShader.h"

namespace GameEngine
{
SimpleEntityShader::SimpleEntityShader(IGraphicsApiPtr graphicsApi)
    : EntityShader(graphicsApi)
{
}

void SimpleEntityShader::Init()
{
    SetFiles({{"Simple/SimpleEntityShader.vert", ShaderType::VERTEX_SHADER},
              {"Simple/SimpleEntityShader.frag", ShaderType::FRAGMENT_SHADER}});

    ShaderProgram::Init();
}
}  // namespace GameEngine
