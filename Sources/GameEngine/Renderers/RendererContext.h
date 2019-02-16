#pragma once
#include <functional>
#include <memory>
#include "RendererFunctionType.h"
#include "Types.h"

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
class IShaderFactory;

struct RendererContext
{
    RendererContext(Projection& projection, GraphicsApi::IGraphicsApi& api,
                    IFrameBuffer& defferedBuffer,
                    IShadowFrameBuffer& shadowBuffer, IShaderFactory& shaderFactory,
                    std::function<void(RendererFunctionType, RendererFunction)> registerFunction)
        : projection_(projection)
        , graphicsApi_(api)
        , shaderFactory_(shaderFactory)
        , defferedFrameBuffer_(defferedBuffer)
        , shadowsFrameBuffer_(shadowBuffer)
        , registerFunction_(registerFunction)
    {
    }
    Projection& projection_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    mat4 toShadowMapZeroMatrix_;
    IShaderFactory& shaderFactory_;
    IFrameBuffer& defferedFrameBuffer_;
    IShadowFrameBuffer& shadowsFrameBuffer_;
    std::function<void(RendererFunctionType, RendererFunction)> registerFunction_;
};
}  // namespace GameEngine
#define __RegisterRenderFunction__(x, y) \
    context_.registerFunction_(x, std::bind(&y, this, std::placeholders::_1, std::placeholders::_2))
