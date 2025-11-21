#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "IRendererFactory.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class RendererFactory : public IRendererFactory
{
public:
    RendererFactory(GraphicsApi::IGraphicsApi&);
    std::unique_ptr<IRenderer> create(RendererContext&, GraphicsApi::IFrameBuffer&) const override;

private:
    GraphicsApi::IGraphicsApi& graphicsApi;
};
}  // namespace GameEngine