#pragma once
#include <memory>

#include "GameEngine/Renderers/Framebuffer/Postprocessing/PostproccesFrameBuffer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
class PostprocessingRenderer
{
public:
    PostprocessingRenderer(RendererContext& rendererContext, PostprocessFrameBuffer** postprocessFrameBuffer)
        : rendererContext_(rendererContext)
        , postprocessFrameBuffer_(postprocessFrameBuffer)
    {
    }

    virtual ~PostprocessingRenderer() = default;

    virtual void Init()               = 0;
    virtual void Prepare()            = 0;
    virtual void Render(const Scene&) = 0;
    virtual void ReloadShaders()      = 0;

protected:
    RendererContext& rendererContext_;
    PostprocessFrameBuffer** postprocessFrameBuffer_;
};

typedef std::unique_ptr<PostprocessingRenderer> PostprocessingRendererPtr;
}  // namespace GameEngine
