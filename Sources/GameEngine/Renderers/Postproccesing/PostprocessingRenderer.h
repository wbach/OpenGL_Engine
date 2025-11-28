#pragma once
#include <memory>

namespace GameEngine
{
class Scene;
struct RendererContext;
class PostprocessingRenderer
{
public:
    PostprocessingRenderer(RendererContext& rendererContext)
        : rendererContext_(rendererContext)
    {
    }

    virtual ~PostprocessingRenderer() = default;

    virtual void Init()               = 0;
    virtual void Prepare()            = 0;
    virtual void Render(const Scene&) = 0;
    virtual void ReloadShaders()      = 0;
    virtual void CleanUp()            = 0;

protected:
    RendererContext& rendererContext_;
};

typedef std::unique_ptr<PostprocessingRenderer> PostprocessingRendererPtr;
}  // namespace GameEngine
