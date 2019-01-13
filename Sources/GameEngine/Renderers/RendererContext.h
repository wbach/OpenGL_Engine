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
class IShaderFactory;

struct RendererContext
{
    RendererContext(Projection& projection,
                    IGraphicsApi& api,
                    IFrameBuffer& defferedBuffer,
                    IShadowFrameBuffer& shadowBuffer,
                    IShaderFactory& shaderFactory,
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
    IGraphicsApi& graphicsApi_;
    mat4 toShadowMapZeroMatrix_;
    IShaderFactory& shaderFactory_;
    IFrameBuffer& defferedFrameBuffer_;
    IShadowFrameBuffer& shadowsFrameBuffer_;
    std::function<void(RendererFunctionType, RendererFunction)> registerFunction_;
};
}  // namespace GameEngine
#define __RegisterRenderFunction__(x, y) context_.registerFunction_(x, std::bind(&y, this, std::placeholders::_1))
