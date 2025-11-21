#include "RendererFactory.h"

#include <Logger/Log.h>

#include "DefferedRenderer.h"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
RendererFactory::RendererFactory(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi(graphicsApi)
{
}
std::unique_ptr<IRenderer> RendererFactory::create(RendererContext& rendererContext, GraphicsApi::IFrameBuffer& renderTarget) const
{
    const auto rendererType = EngineConf.renderer.type.get();
    auto supportedRenderers = graphicsApi.GetSupportedRenderers();

    if (supportedRenderers.empty())
    {
        LOG_ERROR << "Graphics api not supporting any renderer!";
        return nullptr;
    }

    if (rendererType == GraphicsApi::RendererType::SIMPLE)
    {
        auto iter = std::find(supportedRenderers.begin(), supportedRenderers.end(), GraphicsApi::RendererType::SIMPLE);
        if (iter != supportedRenderers.end())
        {
            LOG_DEBUG << "Create base renderer";
            return std::make_unique<BaseRenderer>(rendererContext, renderTarget);
        }
        else
        {
            LOG_DEBUG << "Graphics api are not supporting SIMPLE renderer try using full";
            LOG_DEBUG << "Create deffered renderer";
            return std::make_unique<DefferedRenderer>(rendererContext, renderTarget);
        }
    }

    if (rendererType == GraphicsApi::RendererType::FULL)
    {
        auto iter = std::find(supportedRenderers.begin(), supportedRenderers.end(), GraphicsApi::RendererType::FULL);
        if (iter != supportedRenderers.end())
        {
            LOG_DEBUG << "Create deffered renderer";
            return std::make_unique<DefferedRenderer>(rendererContext, renderTarget);
        }
        else
        {
            LOG_DEBUG << "Graphics api are not supporting FULL renderer try using simple";
            LOG_DEBUG << "Create base renderer";
            return std::make_unique<BaseRenderer>(rendererContext, renderTarget);
        }
    }
    return nullptr;
}
}  // namespace GameEngine
