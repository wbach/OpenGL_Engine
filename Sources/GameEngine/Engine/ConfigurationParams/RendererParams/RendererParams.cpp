#include "RendererParams.h"

namespace GameEngine
{
namespace Params
{
namespace
{
const vec2ui DEFAULT_WINDOW_SIZE = {1000, 600};
std::vector<std::string> avaiablesApis()
{
    std::vector<std::string> result{{"OpenGL"}};
#ifndef USE_GNU
    result.push_back({"DirectX11"});
#endif
    return result;
}
}  // namespace
Renderer::Renderer()
    : graphicsApi{IConfigurationParam::Type::String, {"OpenGL"}, avaiablesApis()}
    , type{IConfigurationParam::Type::Enum,
           GraphicsApi::RendererType::FULL,
           {GraphicsApi::RendererType::SIMPLE, GraphicsApi::RendererType::FULL}}
    , viewDistance{IConfigurationParam::Type::Float,
                   3000.f,
                   {800.f, 1000.f, 1200.f, 1500.f, 2000.f, 3000.f, 4000.f, 5000.f, 10000.f}}
    , normalMappingDistance{IConfigurationParam::Type::Float, 200.f, {100.f, 200.f, 400.f}}
    , fpsLimt{IConfigurationParam::Type::Integer, 60, {30, 60, 75, 144}}
    , resolution{IConfigurationParam::Type::Vec2i, DEFAULT_WINDOW_SIZE, {DEFAULT_WINDOW_SIZE}}
{
}
}  // namespace Params
}  // namespace GameEngine
