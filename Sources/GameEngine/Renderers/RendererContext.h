#pragma once
#include <functional>
#include <memory>

#include "RendererFunctionType.h"
#include "Types.h"

namespace Utils
{
class MeasurementHandler;
} // namespace Utils

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi

namespace GameEngine
{
struct Time;
class Scene;
class Projection;
class IFrameBuffer;
class IShadowFrameBuffer;
class PostprocessFrameBuffer;
class Frustrum;

struct RendererContext
{
    RendererContext(Projection& projection, Frustrum& frustrum, GraphicsApi::IGraphicsApi& api,
                    IFrameBuffer& defferedBuffer, IShadowFrameBuffer& shadowBuffer,
                    Utils::MeasurementHandler& measurmentHandler,
                    std::function<void(RendererFunctionType, RendererFunction)> registerFunction)
        : projection_(projection)
        , frustrum_(frustrum)
        , graphicsApi_(api)
        , defferedFrameBuffer_(defferedBuffer)
        , shadowsFrameBuffer_(shadowBuffer)
        , measurmentHandler_(measurmentHandler)
        , registerFunction_(registerFunction)
    {
    }
    Projection& projection_;
    Frustrum& frustrum_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    mat4 toShadowMapZeroMatrix_ = glm::mat4(1.f);
    IFrameBuffer& defferedFrameBuffer_;
    IShadowFrameBuffer& shadowsFrameBuffer_;
    Utils::MeasurementHandler& measurmentHandler_;
    std::function<void(RendererFunctionType, RendererFunction)> registerFunction_;
};
}  // namespace GameEngine
#define __RegisterRenderFunction__(x, y) \
    context_.registerFunction_(x, std::bind(&y, this, std::placeholders::_1, std::placeholders::_2))
