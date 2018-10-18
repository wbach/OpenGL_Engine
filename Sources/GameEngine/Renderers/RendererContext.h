#pragma once
#include <functional>
#include <memory>
#include "RendererFunctionType.h"
#include "Types.h"

class CProjection;
class PostprocessFrameBuffer;

namespace GameEngine
{
class IGraphicsApi;
class IFrameBuffer;
class IShadowFrameBuffer;

struct RendererContext
{
    RendererContext()
    {
    }
    RendererContext(CProjection* projection, const std::shared_ptr<IGraphicsApi>& api,
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
    CProjection* projection_;
    mat4 toShadowMapZeroMatrix_;
    std::shared_ptr<IGraphicsApi> graphicsApi_;
    std::shared_ptr<IFrameBuffer> defferedFrameBuffer_;
    std::shared_ptr<IShadowFrameBuffer> shadowsFrameBuffer_;
    std::function<void(RendererFunctionType, RendererFunction)> registerFunction_;
};
}  // GameEngine
#define __RegisterRenderFunction__(x, y) context_.registerFunction_(x, std::bind(&y, this, std::placeholders::_1))
