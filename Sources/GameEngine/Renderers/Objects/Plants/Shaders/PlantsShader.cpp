#include "PlantsShader.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace GameEngine
{
PlantsShader::PlantsShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::ShaderProgramType::Plants)
{

}
} // namespace GameEngine
