#include "RendererParams.h"
#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
namespace
{
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
    : graphicsApi{{"OpenGL"}, avaiablesApis()}
    , type{GraphicsApi::RendererType::FULL, {GraphicsApi::RendererType::SIMPLE, GraphicsApi::RendererType::FULL}}
    , viewDistance{3000.f,
                   {50.f, 100.f, 200.f, 400.f, 800.f, 1000.f, 1200.f, 1500.f, 2000.f, 3000.f, 4000.f, 5000.f, 10000.f}}
    , normalMappingDistance{200.f, {100.f, 200.f, 400.f}}
    , fpsLimt{60, {30, 60, 75, 144}}
    , resolution{DEFAULT_WINDOW_SIZE, defaultResolutions()}
{
}
}  // namespace Params
}  // namespace GameEngine
