#pragma once
#include <functional>
#include <memory>
#include "RendererFunctionType.h"
#include "Types.h"

namespace GameEngine
{
class Projection;
class IGraphicsApi;
class IFrameBuffer;
class IShadowFrameBuffer;
class PostprocessFrameBuffer;

struct RendererContext
{
    RendererContext()
    {
    }
    RendererContext(Projection* projection, const std::shared_ptr<IGraphicsApi>& api,
                    const std::shared_ptr<IFrameBuffer>& defferedBuffer,
                    const std::shared_ptr<IShadowFrameBuffer>& shadowBuffer,
                    std::function<void(RendererFunctionType, RendererFunction)> registerFunction)
        : projection_(projection)
        , graphicsApi_(api)
        , defferedFrameBuffer_(defferedBuffer)
        , shadowsFrameBuffer_(shadowBuffer)
        , registerFunction_(registerFunction)
    {
    }
    Projection* projection_;
    mat4 toShadowMapZeroMatrix_;
    std::shared_ptr<IGraphicsApi> graphicsApi_;
    std::shared_ptr<IFrameBuffer> defferedFrameBuffer_;
    std::shared_ptr<IShadowFrameBuffer> shadowsFrameBuffer_;
    std::function<void(RendererFunctionType, RendererFunction)> registerFunction_;
};
}  // GameEngine
#define __RegisterRenderFunction__(x, y) context_.registerFunction_(x, std::bind(&y, this, std::placeholders::_1))
