#pragma once
#include <memory>

namespace GraphicsApi
{
class IFrameBuffer;
}

namespace GameEngine
{
class IRenderer;
struct RendererContext;

class IRendererFactory
{
public:
    virtual ~IRendererFactory() = default;

    virtual std::unique_ptr<IRenderer> create(RendererContext&, GraphicsApi::IFrameBuffer&) const = 0;
};
}  // namespace GameEngine